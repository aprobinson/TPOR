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
#include <boost/shared_ptr.hpp>
#ifdef USEMOAB
 #include <MBInterface.hpp>
 #include <MBCore.hpp>
 #include <MBRange.hpp>
#endif

//MOAB Error Handling
#ifdef USEMOAB
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
public:
  int id;
  int xindex;
  int yindex;
  int zindex;
  SeedType type;
  double angleindex;
  double value;
  double wtdvalue;
  friend inline bool operator < (const ratio &r1, const ratio &r2){
    return r1.wtdvalue < r2.wtdvalue;
  }
  friend inline bool operator > (const ratio &r1, const ratio &r2){
    return r1.wtdvalue > r2.wtdvalue;
  }
  friend inline ratio operator * (const ratio &r1, const ratio &r2){
    ratio newr = r1;
    newr.wtdvalue = r1.wtdvalue*r2.wtdvalue;
    return newr;
  }
  inline bool operator == (const ratio &that) const{
    //need to make this more robust!
    return this->id == that.id;
  }
  ratio & operator= (const ratio& that){
    if(this != &that){
      id = that.id;
      xindex = that.xindex;
      yindex = that.yindex;
      zindex = that.zindex;
      type = that.type;
      angleindex = that.angleindex;
      value = that.value;
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

double map_dose(const int dosestart,
		const int xseedpts,
		const int yseedpts,
		const int zseedpts,
		const int xseedstart,
		const int yseedstart,
		const int zseedstart,
		const int xmeshsize,
		const int ymeshsize,
		const int zmeshsize,
		const int Dp,
		std::vector<double>& mappeddose,
		const std::vector<double>& dosedist,
		const std::vector<int>& target)
{
  int numV100 = 0;
  for(int n=zseedstart, Fk=n+zmeshsize, kl=0; n<Fk; n++, kl++){
    for(int m=yseedstart, Fj=m+ymeshsize, jl=0; m<Fj; m++, jl++){
      for(int l=xseedstart, Fi=l+xmeshsize, il=0; l<Fi; l++, il++){
	mappeddose[il+xmeshsize*jl+xmeshsize*ymeshsize*kl] = 
	  dosedist[dosestart+l+xseedpts*m+xseedpts*yseedpts*n];
	if(mappeddose[il+xmeshsize*jl+xmeshsize*ymeshsize*kl]*
	   target[il+xmeshsize*jl+xmeshsize*ymeshsize*kl] >= Dp)
	  numV100++;
      }
    }
  }
  return numV100;
}

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

  //treatment planning type (directional variance/weight factors)
  bool usedirvar = false;
  double varcut = 0.65;
  bool penalize = true;
  double npen;

  //variables and containers for optimization
  std::list<ratio> templateratios;
  std::list<ratio> resetratios;
  std::map<int,double> ratioresetmap;
  std::list<ratio>::iterator sit, eit, rit;
  std::vector<double> adjoint_ratios;
  std::vector<int> urethra;
  std::vector<double> urethra_adjoint;
  int urethra_vol;
  std::vector<int> rectum;
  std::vector<double> rectum_adjoint;
  int rectum_vol;
  std::vector<int> normal;
  std::vector<double> normal_adjoint;
  int normal_vol;
  std::vector<int> margin;
  std::vector<double> margin_adjoint;
  int margin_vol;
  std::vector<int> target;
  std::vector<double> target_adjoint;
  int target_vol;
  std::vector<int> needle_template;
  int xneedlepts, yneedlepts, zneedlepts;
  int xmeshpts, ymeshpts, zmeshpts;
  int angles, angles_s;
  int totS;
  int totN;
  double dxo, dyo, dzo;
  double dxn, dyn, dzn;
  double dxs, dys, dzs;
  double dtheta, dtheta_s;
  int xisoseedpts, yisoseedpts, zisoseedpts;
  int xisoseedcent, yisoseedcent, zisoseedcent;
  int xanisoseedpts, yanisoseedpts, zanisoseedpts;
  int xanisoseedcent, yanisoseedcent, zanisoseedcent;
  std::vector<double> dosedata;

  //solution data
  std::list<ratio> solution;
  std::map<int,int> needles;
  std::vector<double> currentdose;
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
  if(argc != 8){
    printf("Please specify:\n\t1.) the organ mesh file\n\t2.) the template file\n\t3.) the isotropic adjoint data file\n\t4.) the anisotropic adjoint data file\n\t5.) the isotropic seed mesh file\n\t6.) the anisotropic seed mesh file\n\t7.) the run mode (novar or var)\n");
    exit(1);
  }
  else{
    organmeshfile = fopen(argv[1],"r");
    templatefile = fopen(argv[2],"r");
    isoadjointdatafile = fopen(argv[3],"r");
    anisoadjointdatafile = fopen(argv[4],"r");
    isoseedmeshfile = fopen(argv[5],"r");
    anisoseedmeshfile = fopen(argv[6],"r");
    if(strcmp(argv[7],"var") == 0)
      usedirvar = true;
    else if(strcmp(argv[7],"wgt") == 0)
      usedirvar = false;
    else{
      printf("run mode can only be wgt or var\n");
      exit(1);
    }
  }
  printf("reading in data from files\n");

  //-------------------READ IN DATA FROM FILES--------------------------------//
  char *dummy = (char*) malloc(256*sizeof(char));
  int frerr;
  frerr = fscanf(organmeshfile,"%s %i %s %lf %s %i %s %lf %s %i %s %lf",
	 dummy,&xmeshpts,
	 dummy,&dxo,
	 dummy,&ymeshpts,
	 dummy,&dyo,
	 dummy,&zmeshpts,
	 dummy,&dzo);
  //printf("%i %lf %i %lf %i %lf\n",xmeshpts,dxo,ymeshpts,dyo,zmeshpts,dzo);
  frerr = fscanf(organmeshfile,"%s %i",dummy,&target_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    frerr = fscanf(organmeshfile,"%i",&tmp);
    target.push_back(tmp);
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&urethra_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    frerr = fscanf(organmeshfile,"%i",&tmp);
    urethra.push_back(tmp);
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&rectum_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    frerr = fscanf(organmeshfile,"%i",&tmp);
    rectum.push_back(tmp);
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&normal_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    frerr = fscanf(organmeshfile,"%i",&tmp);
    normal.push_back(tmp);
  }
  frerr = fscanf(organmeshfile,"%s %i",dummy,&margin_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    frerr = fscanf(organmeshfile,"%i",&tmp);
    margin.push_back(tmp);
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
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    frerr = fscanf(templatefile,"%i",&tmp);
    needle_template.push_back(tmp);
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
  for(int j=0; j<angles; j++){
    frerr = fscanf(anisoadjointdatafile,"%s %s %s %s %s",dummy,dummy,dummy,dummy,dummy);
    for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
      double tmp;
      frerr = fscanf(anisoadjointdatafile,"%lf",&tmp);
      target_adjoint.push_back(tmp);
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
      double tmp;
      frerr = fscanf(anisoadjointdatafile,"%lf",&tmp);
      urethra_adjoint.push_back(tmp);
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
      double tmp;
      frerr = fscanf(anisoadjointdatafile,"%lf",&tmp);
      rectum_adjoint.push_back(tmp);
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
      double tmp;
      frerr = fscanf(anisoadjointdatafile,"%lf",&tmp);
      normal_adjoint.push_back(tmp);
    }
    frerr = fscanf(anisoadjointdatafile,"%s %s",dummy,dummy);
    for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
      double tmp;
      frerr = fscanf(anisoadjointdatafile,"%lf",&tmp);
      margin_adjoint.push_back(tmp);
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
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    frerr = fscanf(isoadjointdatafile,"%lf",&tmp);
    target_adjoint.push_back(tmp);
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    frerr = fscanf(isoadjointdatafile,"%lf",&tmp);
    urethra_adjoint.push_back(tmp);
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    frerr = fscanf(isoadjointdatafile,"%lf",&tmp);
    rectum_adjoint.push_back(tmp);
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    frerr = fscanf(isoadjointdatafile,"%lf",&tmp);
    normal_adjoint.push_back(tmp);
  }
  frerr = fscanf(isoadjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    frerr = fscanf(isoadjointdatafile,"%lf",&tmp);
    margin_adjoint.push_back(tmp);
  }
  fclose(isoadjointdatafile);

  //read in anisotropci seed dose distribution data from seed mesh file
  frerr = fscanf(anisoseedmeshfile,"%s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %lf",     dummy, &xanisoseedpts,
	 dummy, &xanisoseedcent,
	 dummy, &dxs,
	 dummy, &yanisoseedpts,
	 dummy, &yanisoseedcent,
	 dummy, &dys,
	 dummy, &zanisoseedpts,
	 dummy, &zanisoseedcent,
	 dummy, &dzs,
	 dummy, &angles_s,
	 dummy, &dtheta_s);
  //algorithm assumes that the seed mesh elements and the organ mesh elements
  // are identical - if not, quit
  if(dxs != dyo || dys != dyo || dzs != dzo){
    printf("error: seed mesh elements are different than organ mesh elements\n");
    exit(1);
  }
  if(angles != angles_s){
    printf("error: the directional adjoint data has a different number of angles than the directional seed data\n");
    exit(1);
  }
  for(int j=0; j<angles; j++){
    frerr = fscanf(anisoseedmeshfile,"%s %s %s",dummy,dummy,dummy);
    for(int i=0; i<xanisoseedpts*yanisoseedpts*zanisoseedpts; i++){
      double tmp;
      frerr = fscanf(anisoseedmeshfile,"%lf",&tmp);
      dosedata.push_back(tmp*str);
    }
  }
  fclose(anisoseedmeshfile);

  //read in isotropic seed dose distribution data from seed mesh file
  frerr = fscanf(isoseedmeshfile,"%s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %i %s %lf",     dummy, &xisoseedpts,
	 dummy, &xisoseedcent,
	 dummy, &dxs,
	 dummy, &yisoseedpts,
	 dummy, &yisoseedcent,
	 dummy, &dys,
	 dummy, &zisoseedpts,
	 dummy, &zisoseedcent,
	 dummy, &dzs);
  //algorithm assumes that the seed mesh elements and the organ mesh elements
  // are identical - if not, quit
  if(dxs != dyo || dys != dyo || dzs != dzo){
    printf("error: seed mesh elements are different than organ mesh elements\n");
    exit(1);
  }
  for(int i=0; i<xisoseedpts*yisoseedpts*zisoseedpts; i++){
    double tmp;
    frerr = fscanf(isoseedmeshfile,"%lf",&tmp);
    dosedata.push_back(tmp*str);
  }
  fclose(isoseedmeshfile);
  
  free(dummy);
  
  clock_t end_read = clock();
#ifdef USEMOAB
//------------------CREATE ORGAN MESH IN MOAB-------------------------------//
  for(int i=0; i<=zmeshpts; i++){
    for(int j=0; j<=ymeshpts; j++){
      for(int k=0; k<=xmeshpts; k++){
	coords.push_back(k*dxo);
	coords.push_back(j*dyo);
	coords.push_back(i*dzo);
      }
    }
  }
  int num_verts = (xmeshpts+1)*(ymeshpts+1)*(zmeshpts+1);
  HANDLE_ERROR(d_MBI->create_vertices(&coords[0], num_verts, d_vtx_range));

  // create the hexahedrons
  moab::EntityHandle conn[8];
  for(int i=0; i<zmeshpts; i++){
    for(int j=0; j<ymeshpts; j++){
      for(int k=0; k<xmeshpts; k++){
	int index = k+j*(xmeshpts+1)+i*(xmeshpts+1)*(ymeshpts+1);
	conn[0] = d_vtx_range[index];
	conn[1] = d_vtx_range[index+1];
	conn[2] = d_vtx_range[index+(xmeshpts+1)+1];
	conn[3] = d_vtx_range[index+(xmeshpts+1)];
	conn[4] = d_vtx_range[index+(xmeshpts+1)*(ymeshpts+1)];
	conn[5] = d_vtx_range[index+(xmeshpts+1)*(ymeshpts+1)+1];
	conn[6] = d_vtx_range[index+(xmeshpts+1)*(ymeshpts+1)+(xmeshpts+1)+1];
	conn[7] = d_vtx_range[index+(xmeshpts+1)*(ymeshpts+1)+(xmeshpts+1)];

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
  adjoint_ratios.resize(target_adjoint.size()*(angles+1));
  double Wur=1.0, Wre=1.0, Wma=1.0, Wta=1.0;
  std::vector<double> dirvar;
  if(usedirvar){
    //calculate the variance of the directional adjoint ratios to determine
    //optimal locations for isotropic seed placement;
    std::vector<double> dirave;
    int step = xmeshpts*ymeshpts*zmeshpts;
    dirvar.resize(step);
    fill(dirvar.begin(),dirvar.end(),0.0);
    dirave.resize(step);
    fill(dirave.begin(),dirave.end(),0.0);
    for(int j=0; j<angles; j++){
      for(int i=0; i<step; i++){
	adjoint_ratios[i+j*step] = 
	  (Wur*urethra_adjoint[i+j*step]+Wre*rectum_adjoint[i+j*step]+
	   Wma*margin_adjoint[i+j*step])/(Wta*target_adjoint[i+j*step]);
	dirvar[i] += adjoint_ratios[i+j*step]*adjoint_ratios[i+j*step];
	dirave[i] += adjoint_ratios[i+j*step];
      }
    }
    transform(dirvar.begin(),dirvar.end(),dirvar.begin(),
	      op_const_div<double>((double)angles-1));
    transform(dirave.begin(),dirave.end(),dirave.begin(),
	     op_square<double>());
    transform(dirave.begin(),dirave.end(),dirave.begin(),
	      op_const_div<double>((double)angles*(angles-1)));
    transform(dirvar.begin(),dirvar.end(),dirave.begin(),
	      dirvar.begin(),op_sub<double>());
    dirave.clear();
    //calculate the adjoint ratios for the isotropic seed
    for(int i=0; i<step; i++){
      adjoint_ratios[i+step*angles] = 
	(Wur*urethra_adjoint[i+step*angles]+Wre*rectum_adjoint[i+step*angles]+
	   Wma*margin_adjoint[i+step*angles])/
	(Wta*target_adjoint[i+step*angles]);
    }
  }
  else{
    int step = xmeshpts*ymeshpts*zmeshpts;
    for(int j=0; j<angles; j++){
      for(int i=0; i<step; i++){
	adjoint_ratios[i+j*step] = 
	  (Wur*urethra_adjoint[i+j*step]+Wre*rectum_adjoint[i+j*step]+
	   Wma*margin_adjoint[i+j*step])/(Wta*target_adjoint[i+j*step]);
      }
    }
    //use different weight for the isotropic seed - trial and error
    Wur = 0.5;
    Wre = 1.0;
    Wma = 0.5;
    Wta = 1.0;
    for(int i=0; i<step; i++){
      adjoint_ratios[i+step*angles] = (Wur*urethra_adjoint[i+step*angles]+
				       Wre*rectum_adjoint[i+step*angles]+
				       Wma*margin_adjoint[i+step*angles])/
	(Wta*target_adjoint[i+step*angles]);
    }
  }  
  ratio tmpratio;
  double minval;
  double tmpval;
  double curval;
  int tmpl;
  int id = 0;
  for(int k=0; k<zmeshpts; k++){
    for(int j=4; j<ymeshpts; j+=5){
      for(int i=4; i<xmeshpts; i+=5){
	if(needle_template[i+xmeshpts*j+xmeshpts*ymeshpts*k] == 1){
	  tmpratio.id = id;
	  id++;
	  tmpratio.xindex = i;
	  tmpratio.yindex = j;
	  tmpratio.zindex = k;
	  if(usedirvar){
	    if(dirvar[i+xmeshpts*j+xmeshpts*ymeshpts*k] < varcut){
	      tmpratio.type = ISO;
	      tmpratio.angleindex = angles;
	      tmpratio.value = adjoint_ratios[i+xmeshpts*j+xmeshpts*ymeshpts*k+
					      xmeshpts*ymeshpts*zmeshpts*
					      angles];
	      tmpratio.wtdvalue = tmpratio.value;
	      templateratios.push_back(tmpratio);
	      continue;
	    }
	  }
	  minval = std::numeric_limits<double>::infinity();
	  for(int l=0; l<angles+1; l++){
	    tmpval = adjoint_ratios[i+xmeshpts*j+xmeshpts*ymeshpts*k+
				    xmeshpts*ymeshpts*zmeshpts*l];
	    if(tmpval < minval){
	      minval = tmpval;
	      tmpl = l;
	    }
	  }
	  if(tmpl == angles){
	    tmpratio.type = ISO;
	    tmpratio.angleindex = angles;
	  }
	  else{
	    tmpratio.type = ANISO;
	    tmpratio.angleindex = tmpl;
	  }
	  tmpratio.value = minval;
	  tmpratio.wtdvalue = minval;
	  templateratios.push_back(tmpratio);
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
  dirvar.clear();
  
  //don't sort the template ratios since they sort keys (value) will change
  // after each interation
  //templateratios.sort();

  //number of needles chosen
  int numN = 0;

  //number of seeds chosen
  int numS = 0;

  //current seed index
  int Si, Sj, Sk, Sa;

  //current seed type
  SeedType St;

  currentdose.resize(target.size());
  fill(currentdose.begin(),currentdose.end(),0.0);

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
  needles[Si+xmeshpts*Sj] = 1;

  //map dose from first seed
  if(St == ISO){ 
    for(int n=zisoseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
      for(int m=yisoseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
	for(int l=xisoseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
	  currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] = 
	    dosedata[l+xisoseedpts*m+xisoseedpts*yisoseedpts*n+
		     Sa*xanisoseedpts*yanisoseedpts*zanisoseedpts];
	}
      }
    }
  }
  else{
    for(int n=zanisoseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
      for(int m=yanisoseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
	for(int l=xanisoseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
	  currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] = 
	    dosedata[l+xanisoseedpts*m+xanisoseedpts*yanisoseedpts*n+
		     Sa*xanisoseedpts*yanisoseedpts*zanisoseedpts];
	}
      }
    }
  }

#ifdef USEMOAB
  if(usedirvar)
    fname_base.append("_var_step");
  else
    fname_base.append("_wgt_step");

  seed_pts.resize(target.size());
  seed_pts[Si+xmeshpts*Sj+xmeshpts*ymeshpts*Sk] = Sa+1;
  
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
  sprintf(frame,"%i",numS);
  fname.append(frame);
  fname.append(".vtk");

  HANDLE_ERROR(d_MBI->write_mesh(fname.c_str()));
#endif  

  //keep selecting seeds until 98% target coverage is achieved
  while((double)numV100/target_vol < 0.98 && templateratios.size() > 0){ 
    sit = templateratios.begin();
    eit = templateratios.end();
    //update adjoint ratios by multiplying by current relative dose
    while(sit != eit){
      (*sit).wtdvalue = (*sit).value*
	currentdose[(*sit).xindex + xmeshpts*(*sit).yindex +
		    xmeshpts*ymeshpts*(*sit).zindex]/Dp;
      sit++;
    }

    sit = min_element(templateratios.begin(),templateratios.end());
    Si = (*sit).xindex;
    Sj = (*sit).yindex;
    Sk = (*sit).zindex;
    Sa = (*sit).angleindex;
    St = (*sit).type;
    
    while(penalize){
      //no penalty for using the same needle
      if(needles.count(Si+xmeshpts*Sj) == 1){
	solution.push_back(*sit);
	sit = templateratios.erase(sit);
	break;
      }
      
      //need to penalize for choosing a new needle
      else{
	npen = sqrt((30.0*30.0+(numN+1)*(numN+1))/(30*30-(numN+1)*(numN+1)))
	  *0.5;
	(*sit).wtdvalue *= npen;
	eit = min_element(templateratios.begin(),templateratios.end());
	
	//needle penalty had no effect - add new needle
	if(sit == eit){
	  solution.push_back(*sit);
	  needles[Si+xmeshpts*Sj] = numN+1;
	  numN++;
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
    //resetratios.clear();
    numS++;
    
    //map dose from latest seed
    if(St == ISO){ 
      numV100 = 0;
      for(int n=zisoseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
	for(int m=yisoseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
	  for(int l=xisoseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
	    currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] += 
	      dosedata[l+xisoseedpts*m+xisoseedpts*yisoseedpts*n+
		       Sa*xanisoseedpts*yanisoseedpts*zanisoseedpts];
	    if(currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl]*
	       target[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] >= Dp)
	      numV100++;
	  }
	}
      }
    }
    else{
      numV100 = 0;
      for(int n=zanisoseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
	for(int m=yanisoseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
	  for(int l=xanisoseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
	    currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] += 
	      dosedata[l+xanisoseedpts*m+xanisoseedpts*yanisoseedpts*n+
		       Sa*xanisoseedpts*yanisoseedpts*zanisoseedpts];
	    if(currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl]*
	       target[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] >= Dp)
	      numV100++;
	  }
	}
      }
    }
#ifdef USEMOAB
    seed_pts[Si+xmeshpts*Sj+xmeshpts*ymeshpts*Sk] = Sa+1;
  
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
    sprintf(frame,"%i",numS);
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
	   needles.find(sx+xmeshpts*sy)->second,
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
    for(int k=0; k<zmeshpts; k++){
      for(int j=0; j<ymeshpts; j++){
	for(int i=0; i<xmeshpts; i++){
	  if(currentdose[i+j*xmeshpts+k*xmeshpts*ymeshpts] >= dose){
	    if(target[i+j*xmeshpts+k*xmeshpts*ymeshpts] == 1)
	      target_num++;
	    else if(urethra[i+j*xmeshpts+k*xmeshpts*ymeshpts] == 1)
	      urethra_num++;
	    else if(rectum[i+j*xmeshpts+k*xmeshpts*ymeshpts] == 1)
	      rectum_num++;
	    else if(normal[i+j*xmeshpts+k*xmeshpts*ymeshpts] == 1)
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
