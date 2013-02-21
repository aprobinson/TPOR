#include <vector>
#include <map>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <functional>
#include <limits>
#include <time.h>
#include "advanced_shared_array.hpp"

#ifdef USEMOAB
 #include <MBInterface.hpp>
 #include <MBCore.hpp>
 #include <MBRange.hpp>
 #include <boost/shared_ptr.hpp>

//MOAB Error Handling
static void HandleError( moab::ErrorCode err,
			 const char *file,
			 int line )
{
  if(err != moab::MB_SUCCESS){
    printf("error: %s in %s at line %d\n", moab::ErrorCodeStr[err],
	   file, line);
    exit(err);
  }
}

#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))
#endif

//seed type enumeration
enum SeedType{
  SeedType_MIN = 0,
  ISO = SeedType_MIN,
  ANISO,
  SeedType_MAX = ANISO
};

const char* const SeedTypeStr[] = {
  "ISO",
  "ANISO"
};

//stores a voxels indices and its adjoint ratio
class ratio
{
private:
  advanced_shared_array<double> _current_dose;
  advanced_shared_array<double> _seed_dose;
  advanced_shared_array<int> _target;
  advanced_shared_array<int> _seedcenters; //0-x, 1-y, 2-z
  advanced_shared_array<int> _seeddims; //0-x, 1-y, 2-z
  advanced_shared_array<int> _meshdims; //0-x, 1-y, 2-z
  int _Dp; //prescribed dose
  int _id;  //unique ratio id

public:
  int xindex;
  int yindex;
  int zindex;
  SeedType type;
  double angleindex;
  double value;
  double coverage;
  double wtdvalue;

  ratio(advanced_shared_array<double> current_dose,
	advanced_shared_array<double> seed_dose,
	advanced_shared_array<int> target,
	advanced_shared_array<int> seedcenters,
	advanced_shared_array<int> seeddims,
	advanced_shared_array<int> meshdims,
	int Dp, int id) 
  //: _current_dose(current_dose), _seed_dose(seed_dose),
  //  _target(target), _seedcenters(seedcenters),
  //  _seeddims(seeddims), _meshdims(meshdims), _Dp(Dp), _id(id) { }
  { _current_dose = current_dose;
    _seed_dose = seed_dose;
    _target = target;
    _seedcenters = seedcenters;
    _seeddims = seeddims;
    _meshdims = meshdims;
    _Dp = Dp;
    _id = id;
  }
  
  void update()
  {
    coverage = 0.0;
    double tmp;
    int index;
    for(int n=_seedcenters[2]-zindex, Fk=n+_meshdims[2], kl=0; n<Fk; n++, kl++){
      for(int m=_seedcenters[1]-yindex, Fj=m+_meshdims[1], jl=0; m<Fj; m++, 
	    jl++){
	for(int l=_seedcenters[0]-xindex, Fi=l+_meshdims[0], il=0; l<Fi; l++,
	      il++){
	  index = il+_meshdims[0]*jl+_meshdims[0]*_meshdims[1]*kl;
	  if(_target[index] > 0 && _current_dose[index] < _Dp){
	    tmp = _current_dose[index]+
	      _seed_dose[l+_seeddims[0]*m+_seeddims[0]*_seeddims[1]*n];
	    tmp < _Dp ? coverage += tmp - _current_dose[index] : 
	      coverage += _Dp-_current_dose[index];
	  }
	}
      }
    }
    if(coverage == 0.0) wtdvalue = std::numeric_limits<double>::infinity();
    else wtdvalue = value/coverage;
  }
  
  inline friend bool operator < (const ratio &r1, const ratio &r2){
    return r1.wtdvalue < r2.wtdvalue;
  }
  inline friend bool operator > (const ratio &r1, const ratio &r2){
    return r1.wtdvalue > r2.wtdvalue;
  }
  inline friend ratio operator * (const ratio &r1, const ratio &r2){
    ratio newr = r1;
    newr.wtdvalue = r1.wtdvalue*r2.wtdvalue;
    return newr;
  }
  inline bool operator == (const ratio &that) const{
    //need to make this more robust!
    return this->_id == that._id;
  }
  ratio & operator= (const ratio& that){
    if(this != &that){
        _current_dose = that._current_dose;
	_seed_dose = that._seed_dose;
	_target = that._target;
	_seedcenters = that._seedcenters; 
	_seeddims = that._seeddims;
	_meshdims = that._meshdims;
	_Dp = that._Dp;
	_id = that._id;  
	xindex = that.xindex;
	yindex = that.yindex;
	zindex = that.zindex;
	type = that.type;
	angleindex = that.angleindex;
	value = that.value;
	coverage = that.coverage;
	wtdvalue = that.wtdvalue;
    }
    return *this;
  }
};

//unitary operations for use with the transform algorithm
template <typename T> struct op_mult : std::binary_function<T,T,T> {
  inline T operator() (const T& i, const T& j) const
  { return i*j; }
};
template<typename T> struct op_divide : std::binary_function<T,T,T> {
  inline T operator() (const T& i, const T& j) const
  { return i/j; }
};
template<typename T> struct op_add : std::binary_function<T,T,T> {
  inline T operator() (const T& i, const T& j) const
  { return i+j; }
};
template<typename T> struct op_sub : std::binary_function<T,T,T> {
  inline T operator() (const T& i, const T& j) const
  { return i-j;}
};
template<typename T> struct op_const_mult : std::unary_function<T,T>{
  T _const_mult;
  op_const_mult(T const_mult) : _const_mult(const_mult) {}
  inline T operator() (const T& i)
  { return i*_const_mult; }
};
template<typename T> struct op_const_div : std::unary_function<T,T>{
  T _const_div;
  op_const_div(T const_div) : _const_div(const_div) {}
  inline T operator() (const T& i)
  { return i/_const_div; }
};
template<typename T> struct op_square : std::unary_function<T,T> {
  inline T operator() (const T& i)
  { return i*i;}
};

int main(int argc, char** argv)
{
  clock_t start = clock();
  
  //data files
  FILE *organmeshfile;
  FILE *templatefile;
  FILE *isoadjointdatafile;
  FILE *anisoadjointdatafile;
  FILE *isoseedmeshfile;
  FILE *anisoseedmeshfile;

  //prescribed dose (Gy)
  double Dp = 145;
  
  //total decays assuming initial normalization to 1mCi
  double str = 0.43;

  //needle penalty
  double npen;

  //use all ratios?
  bool use_all = true;

  //adjoint ROI weights
  double Wur, Wre, Wma=1.0, Wta=1.0;

  //variables and contianers for optimization
  std::list<ratio> templateratios;
  std::list<ratio>::iterator sit, eit;
  std::vector<advanced_shared_array<double> > adjoint_ratios;
  advanced_shared_array<int> urethra;
  std::vector<advanced_shared_array<double> > urethra_adjoint;
  int urethra_vol;
  advanced_shared_array<int> rectum;
  std::vector<advanced_shared_array<double> > rectum_adjoint;
  int rectum_vol;
  advanced_shared_array<int> normal;
  std::vector<advanced_shared_array<double> > normal_adjoint;
  int normal_vol;
  advanced_shared_array<int> margin;
  std::vector<advanced_shared_array<double> > margin_adjoint;
  int margin_vol;
  advanced_shared_array<int> target;
  std::vector<advanced_shared_array<double> > target_adjoint;
  int target_vol;
  advanced_shared_array<int> needle_template;
  advanced_shared_array<int> needlepts(new int[3], 3);
  advanced_shared_array<int> meshpts(new int[3], 3);
  int angles, angles_s;
  int totS;
  int totN;
  advanced_shared_array<double> organdims(new double[3], 3);
  advanced_shared_array<double> needledims(new double[3], 3);
  advanced_shared_array<double> seeddims(new double[3], 3);
  double dtheta, dtheta_s;
  advanced_shared_array<int> isoseedpts(new int[3], 3);
  advanced_shared_array<int> isoseedcents(new int[3], 3);
  advanced_shared_array<int> anisoseedpts(new int[3], 3);
  advanced_shared_array<int> anisoseedcents(new int[3], 3);
  std::vector<advanced_shared_array<double> > dosedata;

  //solution data
  std::list<ratio> solution;
  std::map<int,int> needles;
  advanced_shared_array<double> currentdose;
  int numV100 = 0;
#ifdef USEMOAB
  //moab data
  std::vector<int> seed_pts; 
  std::vector<double> targetdose; 
  std::vector<double> urethradose; 
  std::vector<double> rectumdose;
  std::vector<double> normaldose;
  std::vector<double> margindose;
  std::string fname_base("treatment_plan"); 
  std::string fname; 
  char* frame = (char*) malloc(256*sizeof(char)); 
  boost::shared_ptr<moab::Interface> d_MBI( new moab::Core() );
  moab::Range d_vtx_range;
  moab::Range d_hex_range;
  moab::ErrorCode rval;
  std::vector<double> coords;
#endif

  //open necessary files
  if(argc != 9){
    printf("Please specify:\n\t1.) the organ mesh file\n\t2.) the template file\n\t3.) the isotropic adjoint data file\n\t4.) the anisotropic adjoint data file\n\t5.) the isotropic seed mesh file\n\t6.) the anisotropic seed mesh file\n\t7.) the urethra weight\n\t8.) the rectum weight\n");
    exit(1);
  }
  else{
    organmeshfile = fopen(argv[1],"r");
    templatefile = fopen(argv[2],"r");
    isoadjointdatafile = fopen(argv[3],"r");
    anisoadjointdatafile = fopen(argv[4],"r");
    isoseedmeshfile = fopen(argv[5],"r");
    anisoseedmeshfile = fopen(argv[6],"r");
    Wur = atof(argv[7]);
    Wre = atof(argv[8]);
  }
  printf("reading in data from files\n");

  //-------------------READ IN DATA FROM FILES--------------------------------//
  char *dummy = (char*) malloc(256*sizeof(char));
  int frerr;
  frerr = fscanf(organmeshfile,"%s %i %s %lf %s %i %s %lf %s %i %s %lf",
	 dummy,&meshpts[0],
	 dummy,&organdims[0],
	 dummy,&meshpts[1],
	 dummy,&organdims[1],
	 dummy,&meshpts[2],
	 dummy,&organdims[2]);
  //printf("%i %lf %i %lf %i %lf\n",meshpts[0],dxo,meshpts[1],dyo,meshpts[2],dzo);
  advanced_shared_array<int>::size_type arsize = 
    meshpts[0]*meshpts[1]*meshpts[2];
  int itmp;
  urethra.reset(new int[arsize], arsize);
  rectum.reset(new int[arsize], arsize);
  normal.reset(new int[arsize], arsize);
  margin.reset(new int[arsize], arsize);
  target.reset(new int[arsize], arsize);
  
  frerr = fscanf(organmeshfile,"%s %i",dummy,&target_vol);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(organmeshfile,"%i",&itmp);
    target[i] = itmp;
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&urethra_vol);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(organmeshfile,"%i",&itmp);
    urethra[i] = itmp;
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&rectum_vol);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(organmeshfile,"%i",&itmp);
    rectum[i] = itmp;
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&normal_vol);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(organmeshfile,"%i",&itmp);
    normal[i] = itmp;
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&margin_vol);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(organmeshfile,"%i",&itmp);
    margin[i] = itmp;
  }
  fclose(organmeshfile);

  //read in template data
  frerr = fscanf(templatefile,"%s %s %s %s %s %s %s %s %s %s %s %s",
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy);

  needle_template.reset(new int[arsize], arsize);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(templatefile,"%i",&itmp);
    needle_template[i] = itmp;
  }
  fclose(templatefile);

  //read in anisotropic adjoint data
  frerr = fscanf(anisoadjointdatafile,"%s %s %s %s %s %s %s %s %s %s %s %s %s %i %s %lf",
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, &angles,
	 dummy, &dtheta);
  
  urethra_adjoint.resize(angles+1);
  rectum_adjoint.resize(angles+1);
  normal_adjoint.resize(angles+1);
  margin_adjoint.resize(angles+1);
  target_adjoint.resize(angles+1);
  double dtmp;
  
  for(int j=0; j<angles; j++){
    urethra_adjoint[j].reset(new double[arsize], arsize);
    rectum_adjoint[j].reset(new double[arsize], arsize);
    normal_adjoint[j].reset(new double[arsize], arsize);
    margin_adjoint[j].reset(new double[arsize], arsize);
    target_adjoint[j].reset(new double[arsize], arsize);
    frerr = fscanf(anisoadjointdatafile,"%s %s %s %s %s",dummy,dummy,dummy,dummy,dummy);
    for(int i=0; i<arsize; i++){
      frerr = fscanf(anisoadjointdatafile,"%lf",&dtmp);
      target_adjoint[j][i] = dtmp;
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<arsize; i++){
      frerr = fscanf(anisoadjointdatafile,"%lf",&dtmp);
      urethra_adjoint[j][i] = dtmp;
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<arsize; i++){
      frerr = fscanf(anisoadjointdatafile,"%lf",&dtmp);
      rectum_adjoint[j][i] = dtmp;
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<arsize; i++){
      frerr = fscanf(anisoadjointdatafile,"%lf",&dtmp);
      normal_adjoint[j][i] = dtmp;
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<arsize; i++){
      frerr = fscanf(anisoadjointdatafile,"%lf",&dtmp);
      margin_adjoint[j][i] = dtmp;
    }
  }
  fclose(anisoadjointdatafile);

  //read in isotropic adjoint data
  frerr = fscanf(isoadjointdatafile,"%s %s %s %s %s %s %s %s %s %s %s %s",
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy);

  urethra_adjoint[angles].reset(new double[arsize], arsize);
  rectum_adjoint[angles].reset(new double[arsize], arsize);
  normal_adjoint[angles].reset(new double[arsize], arsize);
  margin_adjoint[angles].reset(new double[arsize], arsize);
  target_adjoint[angles].reset(new double[arsize], arsize);
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(isoadjointdatafile,"%lf",&dtmp);
    target_adjoint[angles][i] = dtmp;
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(isoadjointdatafile,"%lf",&dtmp);
    urethra_adjoint[angles][i] = dtmp;
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(isoadjointdatafile,"%lf",&dtmp);
    rectum_adjoint[angles][i] = dtmp;
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(isoadjointdatafile,"%lf",&dtmp);
    normal_adjoint[angles][i] = dtmp;
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<arsize; i++){
    frerr = fscanf(isoadjointdatafile,"%lf",&dtmp);
    margin_adjoint[angles][i] = dtmp;
  }
  fclose(isoadjointdatafile);

  //read in anisotropic seed dose distribution data from seed mesh file
  frerr = fscanf(anisoseedmeshfile,"%s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %lf",     dummy, &anisoseedpts[0],
	 dummy, &anisoseedcents[0],
	 dummy, &seeddims[0],
	 dummy, &anisoseedpts[1],
	 dummy, &anisoseedcents[1],
	 dummy, &seeddims[1],
	 dummy, &anisoseedpts[2],
	 dummy, &anisoseedcents[2],
	 dummy, &seeddims[2],
	 dummy, &angles_s,
	 dummy, &dtheta_s);
  //algorithm assumes that the seed mesh elements and the organ mesh elements
  // are identical - if not, quit
  if(seeddims[0] != organdims[0] || 
     seeddims[1] != organdims[1] || 
     seeddims[2] != organdims[2]){
    printf("error: seed mesh elements are different than organ mesh elements\n");
    exit(1);
  }
  if(angles != angles_s){
    printf("error: the directional adjoint data has a different number of angles than the directional seed data\n");
    exit(1);
  }

  int anisoarsize = anisoseedpts[0]*anisoseedpts[1]*anisoseedpts[2];
  dosedata.resize(angles+1);
  for(int j=0; j<angles; j++)
    dosedata[j].reset(new double[anisoarsize], anisoarsize);

  for(int j=0; j<angles; j++){
    frerr = fscanf(anisoseedmeshfile,"%s %s %s",dummy,dummy,dummy);
    for(int i=0; i<anisoarsize; i++){
      frerr = fscanf(anisoseedmeshfile,"%lf",&dtmp);
      dosedata[j][i] = dtmp*str;
    }
  }
  fclose(anisoseedmeshfile);

  //read in isotropic seed dose distribution data from seed mesh file
  frerr = fscanf(isoseedmeshfile,"%s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %i %s %lf",     dummy, &isoseedpts[0],
	 dummy, &isoseedcents[0],
	 dummy, &seeddims[0],
	 dummy, &isoseedpts[1],
	 dummy, &isoseedcents[1],
	 dummy, &seeddims[1],
	 dummy, &isoseedpts[2],
	 dummy, &isoseedcents[2],
	 dummy, &seeddims[2]);
  //algorithm assumes that the seed mesh elements and the organ mesh elements
  // are identical - if not, quit
  if(seeddims[0] != organdims[0] || 
     seeddims[1] != organdims[1] || 
     seeddims[2] != organdims[2]){
    printf("error: seed mesh elements are different than organ mesh elements\n");
    exit(1);
  }
  
  int isoarsize = isoseedpts[0]*isoseedpts[1]*isoseedpts[2];
  dosedata[angles].reset(new double[isoarsize], isoarsize);

  for(int i=0; i<isoarsize; i++){
    frerr = fscanf(isoseedmeshfile,"%lf",&dtmp);
    dosedata[angles][i] = dtmp*str;
  }
  fclose(isoseedmeshfile);
  
  free(dummy);
  
  clock_t end_read = clock();

#ifdef USEMOAB
//------------------CREATE ORGAN MESH IN MOAB-------------------------------//
  for(int i=0; i<=meshpts[2]; i++){
    for(int j=0; j<=meshpts[1]; j++){
      for(int k=0; k<=meshpts[0]; k++){
	coords.push_back(k*organdims[0]);
	coords.push_back(j*organdims[1]);
	coords.push_back(i*organdims[2]);
      }
    }
  }
  int num_verts = (meshpts[0]+1)*(meshpts[1]+1)*(meshpts[2]+1);
  HANDLE_ERROR(d_MBI->create_vertices(&coords[0], num_verts, d_vtx_range));

  // create the hexahedrons
  moab::EntityHandle conn[8];
  for(int i=0; i<meshpts[2]; i++){
    for(int j=0; j<meshpts[1]; j++){
      for(int k=0; k<meshpts[0]; k++){
	int index = k+j*(meshpts[0]+1)+i*(meshpts[0]+1)*(meshpts[1]+1);
	conn[0] = d_vtx_range[index];
	conn[1] = d_vtx_range[index+1];
	conn[2] = d_vtx_range[index+(meshpts[0]+1)+1];
	conn[3] = d_vtx_range[index+(meshpts[0]+1)];
	conn[4] = d_vtx_range[index+(meshpts[0]+1)*(meshpts[1]+1)];
	conn[5] = d_vtx_range[index+(meshpts[0]+1)*(meshpts[1]+1)+1];
	conn[6] = d_vtx_range[index+(meshpts[0]+1)*(meshpts[1]+1)+(meshpts[0]+1)+1];
	conn[7] = d_vtx_range[index+(meshpts[0]+1)*(meshpts[1]+1)+(meshpts[0]+1)];

	moab::EntityHandle this_hex;
	HANDLE_ERROR(d_MBI->create_element(moab::MBHEX, conn, 8, this_hex));
	
	d_hex_range.insert(this_hex);
      }
    }
  }

  // create the organ data tags
  moab::Tag urethra_tag;
  moab::Tag rectum_tag;
  moab::Tag target_tag;
  moab::Tag normal_tag;
  moab::Tag margin_tag;
  moab::Tag template_tag;

  HANDLE_ERROR(d_MBI->tag_create("urethra", sizeof(int), moab::MB_TAG_DENSE,
				 moab::MB_TYPE_INTEGER, urethra_tag, 0));
  
  HANDLE_ERROR(d_MBI->tag_create("rectum", sizeof(int), moab::MB_TAG_DENSE,
				 moab::MB_TYPE_INTEGER, rectum_tag, 0));

  HANDLE_ERROR(d_MBI->tag_create("target", sizeof(int), moab::MB_TAG_DENSE,
				 moab::MB_TYPE_INTEGER, target_tag, 0));
  
  HANDLE_ERROR(d_MBI->tag_create("normal", sizeof(int), moab::MB_TAG_DENSE,
				 moab::MB_TYPE_INTEGER, normal_tag, 0));

  HANDLE_ERROR(d_MBI->tag_create("margin", sizeof(int), moab::MB_TAG_DENSE,
				 moab::MB_TYPE_INTEGER, margin_tag, 0));

  HANDLE_ERROR(d_MBI->tag_create("template", sizeof(int), moab::MB_TAG_DENSE,
				 moab::MB_TYPE_INTEGER, template_tag, 0));

  //tag the hexes
  HANDLE_ERROR(d_MBI->tag_set_data(urethra_tag, d_hex_range, &urethra[0]));

  HANDLE_ERROR(d_MBI->tag_set_data(rectum_tag, d_hex_range, &rectum[0]));

  HANDLE_ERROR(d_MBI->tag_set_data(target_tag, d_hex_range, &target[0]));

  HANDLE_ERROR(d_MBI->tag_set_data(normal_tag, d_hex_range, &normal[0]));

  HANDLE_ERROR(d_MBI->tag_set_data(margin_tag, d_hex_range, &margin[0]));

  HANDLE_ERROR(d_MBI->tag_set_data(template_tag, d_hex_range, 
				   &needle_template[0]));

  //write the organ data vtk file
  HANDLE_ERROR(d_MBI->write_mesh("organ_data.vtk"));

  //remove the organ data tags from MOAB Core
  HANDLE_ERROR(d_MBI->tag_delete(urethra_tag));

  HANDLE_ERROR(d_MBI->tag_delete(rectum_tag));

  HANDLE_ERROR(d_MBI->tag_delete(target_tag));

  HANDLE_ERROR(d_MBI->tag_delete(normal_tag));

  HANDLE_ERROR(d_MBI->tag_delete(margin_tag));

  HANDLE_ERROR(d_MBI->tag_delete(template_tag));

  //create the treatment plan tags
  moab::Tag target_dose;
  moab::Tag urethra_dose;
  moab::Tag rectum_dose;
  moab::Tag normal_dose;
  moab::Tag margin_dose;
  moab::Tag seed_locs;

  HANDLE_ERROR(d_MBI->tag_create("target_dose", sizeof(double), 
				 moab::MB_TAG_DENSE, moab::MB_TYPE_DOUBLE, 
				 target_dose, 0));

  HANDLE_ERROR(d_MBI->tag_create("urethra_dose", sizeof(double),
				 moab::MB_TAG_DENSE, moab::MB_TYPE_DOUBLE,
				 urethra_dose, 0));

  HANDLE_ERROR(d_MBI->tag_create("rectum_dose", sizeof(double),
				 moab::MB_TAG_DENSE, moab::MB_TYPE_DOUBLE,
				 rectum_dose, 0));

  HANDLE_ERROR(d_MBI->tag_create("normal_dose", sizeof(double),
				 moab::MB_TAG_DENSE, moab::MB_TYPE_DOUBLE,
				 normal_dose, 0));

  HANDLE_ERROR(d_MBI->tag_create("margin_dose", sizeof(double),
				 moab::MB_TAG_DENSE, moab::MB_TYPE_DOUBLE,
				 margin_dose, 0));

  HANDLE_ERROR(d_MBI->tag_create("seeds", sizeof(int), 
				 moab::MB_TAG_DENSE, moab::MB_TYPE_INTEGER, 
				 seed_locs, 0));
#endif

  //-------------------START TREATMENT PLANNING ALGORITHM---------------------//
  printf("file reading done - starting treatment planning\n");

  //limit potential seed positions to those that are in the target
  //transform(needle_template.begin(), needle_template.end(), target.begin(),
  //	    needle_template.begin(), op_mult<int>());


  //create the adjoint ratios along the template positions
  adjoint_ratios.resize(angles+1);
  for(int j=0; j<=angles; j++){
    adjoint_ratios[j].reset(new double[arsize], arsize);
    for(int i=0; i<arsize; i++){
      adjoint_ratios[j][i] = (Wur*urethra_adjoint[j][i]+
			      Wre*rectum_adjoint[j][i]+
			      Wma*margin_adjoint[j][i])/
	(Wta*target_adjoint[j][i]);
    }
  }

  //initialize the currentdose array
  currentdose.reset(new double[arsize], arsize);
  std::fill(currentdose.begin(),currentdose.end(),0.0);

  int tmpindex;
  int id=0;
  for(int k=0; k<meshpts[2]; k++){
    for(int j=4; j<meshpts[1]; j+=5){
      for(int i=4; i<meshpts[0]; i+=5){
	tmpindex = i+meshpts[0]*j+meshpts[0]*meshpts[1]*k;
	if(needle_template[tmpindex] == 1){
	  //anisotropic ratios
	  if(use_all == true){
	    for(int a=0; a<angles; a++){
	      ratio tmpratio(currentdose,
			     dosedata[a],
			     target,
			     anisoseedcents,
			     anisoseedpts,
			     meshpts,
			     Dp,
			     id);
	      tmpratio.xindex = i;
	      tmpratio.yindex = j;
	      tmpratio.zindex = k;
	      tmpratio.type = ANISO;
	      tmpratio.angleindex = a;
	      tmpratio.value = adjoint_ratios[a][tmpindex];
	      tmpratio.update();
	      templateratios.push_back(tmpratio);
	    }
	  }
	  else{
	    double minval = std::numeric_limits<double>::infinity();
	    double tmpval;
	    int tmpl;
	    for(int a=0; a<angles; a++){
	      tmpval = adjoint_ratios[a][tmpindex];
	      if(tmpval < minval){
		minval = tmpval;
		tmpl = a;
	      }
	    }
	    ratio tmpratio(currentdose,
			   dosedata[tmpl],
			   target,
			   anisoseedcents,
			   anisoseedpts,
			   meshpts,
			   Dp,
			   id);
	    tmpratio.xindex = i;
	    tmpratio.yindex = j;
	    tmpratio.zindex = k;
	    tmpratio.type = ANISO;
	    tmpratio.angleindex = tmpl;
	    tmpratio.value = tmpval;
	    tmpratio.update();
	    templateratios.push_back(tmpratio);
	  }
	    
	  //isotropic ratio
	  ratio tmpratio(currentdose,
			 dosedata[angles],
			 target,
			 isoseedcents,
			 isoseedpts,
			 meshpts,
			 Dp,
			 id);
	  tmpratio.xindex = i;
	  tmpratio.yindex = j;
	  tmpratio.zindex = k;
	  tmpratio.type = ISO;
	  tmpratio.angleindex = angles;
	  tmpratio.value = adjoint_ratios[angles][tmpindex];
	  tmpratio.update();
	  templateratios.push_back(tmpratio);
	  id++;
	}
      }
    }
  }
  

  //clear unneeded adjoint data
  adjoint_ratios.clear();
  target_adjoint.clear();
  urethra_adjoint.clear();
  rectum_adjoint.clear();
  normal_adjoint.clear();
  margin_adjoint.clear();
  
  //number of needles chosen
  int numN = 0;
  
  //number of seeds chosen
  int numS = 0;

  //current seed index
  int Si, Sj, Sk, Sa;

  //current seed type
  SeedType St;

  //start selection process - select first seed
  sit = min_element(templateratios.begin(),templateratios.end());
  solution.push_back(*sit);
  sit = templateratios.erase(sit);
  Si = solution.back().xindex;
  Sj = solution.back().yindex;
  Sk = solution.back().zindex;
  Sa = solution.back().angleindex;
  St = solution.back().type;
  numN++;
  numS++;

  //assign needle index to needle map
  needles[Si+meshpts[0]*Sj] = numN;

  //map dose from first seed
  if(St == ISO){ 
    for(int n=isoseedcents[2]-Sk, Fk=n+meshpts[2], kl=0; n<Fk; n++, kl++){
      for(int m=isoseedcents[1]-Sj, Fj=m+meshpts[1], jl=0; m<Fj; m++, jl++){
	for(int l=isoseedcents[0]-Si, Fi=l+meshpts[0], il=0; l<Fi; l++, il++){
	  currentdose[il+meshpts[0]*jl+meshpts[0]*meshpts[1]*kl] = 
	    dosedata[Sa][l+isoseedpts[0]*m+isoseedpts[0]*isoseedpts[1]*n];
	}
      }
    }
  }
  else{
    for(int n=anisoseedcents[2]-Sk, Fk=n+meshpts[2], kl=0; n<Fk; n++, kl++){
      for(int m=anisoseedcents[1]-Sj, Fj=m+meshpts[1], jl=0; m<Fj; m++, jl++){
	for(int l=anisoseedcents[0]-Si, Fi=l+meshpts[0], il=0; l<Fi; l++, il++){
	  currentdose[il+meshpts[0]*jl+meshpts[0]*meshpts[1]*kl] = 
	    dosedata[Sa][l+anisoseedpts[0]*m+anisoseedpts[0]*anisoseedpts[1]*n];
	}
      }
    }
  }

#ifdef USEMOAB

  seed_pts.resize(target.size());
  if(St == ISO)
    seed_pts[Si+meshpts[0]*Sj+meshpts[0]*meshpts[1]*Sk] = 1;
  else
    seed_pts[Si+meshpts[0]*Sj+meshpts[0]*meshpts[1]*Sk] = 2;
  
  targetdose.resize(target.size());
  urethradose.resize(urethra.size());
  rectumdose.resize(rectum.size());
  normaldose.resize(normal.size());
  margindose.resize(margin.size());
  transform(currentdose.begin(),currentdose.end(),target.begin(),
	    targetdose.begin(),op_mult<double>());
  transform(currentdose.begin(),currentdose.end(),urethra.begin(),
	    urethradose.begin(),op_mult<double>());
  transform(currentdose.begin(),currentdose.end(),rectum.begin(),
	    rectumdose.begin(),op_mult<double>());
  transform(currentdose.begin(),currentdose.end(),normal.begin(),
	    normaldose.begin(),op_mult<double>());
  transform(currentdose.begin(),currentdose.end(),margin.begin(),
	    margindose.begin(),op_mult<double>());

  //assign data to tags
  HANDLE_ERROR(d_MBI->tag_set_data(seed_locs, d_hex_range, &seed_pts[0]));
  HANDLE_ERROR(d_MBI->tag_set_data(target_dose, d_hex_range, &targetdose[0]));
  HANDLE_ERROR(d_MBI->tag_set_data(urethra_dose, d_hex_range, &urethradose[0]));
  HANDLE_ERROR(d_MBI->tag_set_data(rectum_dose, d_hex_range, &rectumdose[0]));
  HANDLE_ERROR(d_MBI->tag_set_data(normal_dose, d_hex_range, &normaldose[0]));
  HANDLE_ERROR(d_MBI->tag_set_data(margin_dose, d_hex_range, &margindose[0]));

  fname = fname_base;
  sprintf(frame,"-Wur-%1.1lf-Wre-%1.1lf-%i",Wur,Wre,numS);
  fname.append(frame);
  fname.append(".vtk");

  HANDLE_ERROR(d_MBI->write_mesh(fname.c_str()));
#endif  

  //keep selecting seeds until 98% target coverageis achieved
  while((double)numV100/target_vol < 0.98 && templateratios.size() > 0){
    //update all ratio values
    sit = templateratios.begin();
    eit = templateratios.end();
    while(sit != eit){
      if(*sit == solution.back())
	sit = templateratios.erase(sit);
      else{
	(*sit).update();
	sit++;
      }
    }

    sit = min_element(templateratios.begin(),templateratios.end());
    Si = (*sit).xindex;
    Sj = (*sit).yindex;
    Sk = (*sit).zindex;
    Sa = (*sit).angleindex;
    St = (*sit).type;
    solution.push_back(*sit);
    sit = templateratios.erase(sit);
    if(needles.count(Si+meshpts[0]*Sj) == 0){
      numN++;
      needles[Si+meshpts[0]*Sj] = numN;
    }
    
    //penalize for choosing a new needle
    while(false){
      printf("here\n");
      //no penalty for using the same needle
      if(needles.count(Si+meshpts[0]*Sj) == 1){
	solution.push_back(*sit);
	sit = templateratios.erase(sit);
	break;
      }

      //add needle penalty
      else{
	npen = sqrt((30.0*30.0+(numN+1)*(numN+1))/(30*30-(numN+1)*(numN+1)))
	  *0.5;
	(*sit).wtdvalue *= npen;
	eit = min_element(templateratios.begin(),templateratios.end());
	
	//needle penalty had no effect - add new needle
	if(sit == eit){
	  solution.push_back(*sit);
	  numN++;
	  needles[Si+meshpts[0]*Sj] = numN;
	  sit = templateratios.erase(sit);
	  break;
	}
	//needle penalty had an effect
	else{
	  sit = eit;
	  Si = (*sit).xindex;
	  Sj = (*sit).yindex;
	  Sk = (*sit).zindex;
	  Sa = (*sit).angleindex;
	  St = (*sit).type;
	}
      }
    }
    numS++;

    //map dose from latest seed
    if(St == ISO){ 
      numV100 = 0;
      for(int n=isoseedcents[2]-Sk, Fk=n+meshpts[2], kl=0; n<Fk; n++, kl++){
	for(int m=isoseedcents[1]-Sj, Fj=m+meshpts[1], jl=0; m<Fj; m++, jl++){
	  for(int l=isoseedcents[0]-Si, Fi=l+meshpts[0], il=0; l<Fi; l++, il++){
	    tmpindex = il+meshpts[0]*jl+meshpts[0]*meshpts[1]*kl;
	    currentdose[tmpindex] += 
	      dosedata[Sa][l+isoseedpts[0]*m+isoseedpts[0]*isoseedpts[1]*n];
	    if(currentdose[tmpindex]*target[tmpindex] >= Dp)
	      numV100++;
	  }
	}
      }
    }
    else{
      numV100 = 0;
      for(int n=anisoseedcents[2]-Sk, Fk=n+meshpts[2], kl=0; n<Fk; n++, kl++){
	for(int m=anisoseedcents[1]-Sj, Fj=m+meshpts[1], jl=0; m<Fj; m++, jl++){
	  for(int l=anisoseedcents[0]-Si, Fi=l+meshpts[0], il=0; l<Fi; l++, 
		il++){
	    tmpindex = il+meshpts[0]*jl+meshpts[0]*meshpts[1]*kl;
	    currentdose[tmpindex] += 
	      dosedata[Sa][l+anisoseedpts[0]*m+
			   anisoseedpts[0]*anisoseedpts[1]*n];
	    if(currentdose[tmpindex]*target[tmpindex] >= Dp)
	      numV100++;
	  }
	}
      }
    }
    printf("%lf\n",(double)numV100/target_vol);
#ifdef USEMOAB
    if(St == ISO)
      seed_pts[Si+meshpts[0]*Sj+meshpts[0]*meshpts[1]*Sk] = 1;
    else
      seed_pts[Si+meshpts[0]*Sj+meshpts[0]*meshpts[1]*Sk] = 2;
  
    transform(currentdose.begin(),currentdose.end(),target.begin(),
	      targetdose.begin(),op_mult<double>());
    transform(currentdose.begin(),currentdose.end(),urethra.begin(),
	      urethradose.begin(),op_mult<double>());
    transform(currentdose.begin(),currentdose.end(),rectum.begin(),
	      rectumdose.begin(),op_mult<double>());
    transform(currentdose.begin(),currentdose.end(),normal.begin(),
	      normaldose.begin(),op_mult<double>());
    transform(currentdose.begin(),currentdose.end(),margin.begin(),
	      margindose.begin(),op_mult<double>());

    //assign data to tags
    HANDLE_ERROR(d_MBI->tag_set_data(seed_locs, d_hex_range, &seed_pts[0]));
    HANDLE_ERROR(d_MBI->tag_set_data(target_dose, d_hex_range, &targetdose[0]));
    HANDLE_ERROR(d_MBI->tag_set_data(urethra_dose, d_hex_range, &urethradose[0]));
    HANDLE_ERROR(d_MBI->tag_set_data(rectum_dose, d_hex_range, &rectumdose[0]));
    HANDLE_ERROR(d_MBI->tag_set_data(normal_dose, d_hex_range, &normaldose[0]));
    HANDLE_ERROR(d_MBI->tag_set_data(margin_dose, d_hex_range, &margindose[0]));
    
    fname = fname_base;
    sprintf(frame,"-Wur-%1.1lf-Wre-%1.1lf-%i",Wur,Wre,numS);
    fname.append(frame);
    fname.append(".vtk");

    HANDLE_ERROR(d_MBI->write_mesh(fname.c_str()));
#endif      
  }

  clock_t end = clock();
#ifdef USEMOAB
  free(frame);
#else
  //print treatment plan
  printf("numN\tnumS\tSi\tSj\tSk\tType\tAngle\n");
  int seednum;
  int sx,sy,sz,sa;
  SeedType stpe;
  for(sit = solution.begin(), seednum=1; sit != solution.end();sit++,seednum++){
    sx = (*sit).xindex;
    sy = (*sit).yindex;
    sz = (*sit).zindex;
    sa = (*sit).angleindex;
    stpe = (*sit).type;
    printf("%i\t%i\t%i\t%i\t%i\t%s\t%1.1lf\n",
	   needles.find(sx+meshpts[0]*sy)->second,
	   seednum,sx,sy,sz,SeedTypeStr[stpe],(stpe == ISO) ? 
	   0.0 : (sa+1)*dtheta);
  }
  printf("Plan created in %lf (%lf) sec\n",1.0*(end-end_read)/CLOCKS_PER_SEC,
	 1.0*(end-start)/CLOCKS_PER_SEC);

  //print out data for plan evaluation
  FILE *evalfile = fopen("planeval","w");
  fprintf(evalfile,"#Dose[Gy]\tTarget\tUrethra\tRectum\tNormal\n");
  int target_num = 0;
  int urethra_num = 0;
  int rectum_num = 0;
  int normal_num = 0;

  for(int dose=0; dose <= 300; dose++){
    for(int k=0; k<meshpts[2]; k++){
      for(int j=0; j<meshpts[1]; j++){
	for(int i=0; i<meshpts[0]; i++){
	  if(currentdose[i+j*meshpts[0]+k*meshpts[0]*meshpts[1]] >= dose){
	    if(target[i+j*meshpts[0]+k*meshpts[0]*meshpts[1]] == 1)
	      target_num++;
	    else if(urethra[i+j*meshpts[0]+k*meshpts[0]*meshpts[1]] == 1)
	      urethra_num++;
	    else if(rectum[i+j*meshpts[0]+k*meshpts[0]*meshpts[1]] == 1)
	      rectum_num++;
	    else if(normal[i+j*meshpts[0]+k*meshpts[0]*meshpts[1]] == 1)
	      normal_num++;
	  }
	}
      }
    }
    fprintf(evalfile,"%i %lf %lf %lf %lf\n", dose,
	    (double)target_num/target_vol,
	    (double)urethra_num/urethra_vol,
	    (double)rectum_num/rectum_vol,
	    (double)normal_num/normal_vol);
    target_num = 0;
    urethra_num = 0;
    rectum_num = 0;
    normal_num = 0;
  }
  fclose(evalfile);
#endif

  return 0;
}
