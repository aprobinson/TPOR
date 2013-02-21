#include <vector>
#include <map>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <limits>
#include <time.h>

//stores a voxels indices and its adjoint ratio
class ratio
{
public:
  int xindex;
  int yindex;
  int zindex;
  double value;
  friend bool operator < (const ratio &r1, const ratio &r2){
    return r1.value < r2.value;
  }
  friend bool operator > (const ratio &r1, const ratio &r2){
    return r1.value > r2.value;
  }
  friend ratio operator * (const ratio &r1, const ratio &r2){
    ratio newr = r1;
    newr.value = r1.value*r2.value;
    return newr;
  }
  ratio & operator= (const ratio& that){
    if(this != &that){
      xindex = that.xindex;
      yindex = that.yindex;
      zindex = that.zindex;
      value = that.value;
    }
    return *this;
  }
};

//used by std::list to sort the adjoint ratios
bool cmpratio(ratio ratio1, ratio ratio2) 
{ return ratio1.value < ratio2.value; }

//unitary operations for use with the transform algorithm
int op_int_mult(int i, int j){ return i*j; }
int op_int_add(int i, int j){ return i+j; }
int op_int_zero(int i){ return 0; }
double op_double_mult(double i, double j){ return i*j; }
double op_double_add(double i, double j){ return i+j; }
double op_double_divide(double i, double j){ return i/j; }  
double op_double_zero(double i){ return 0.0; }

int main(int argc, char** argv)
{
  clock_t start = clock();
  
  //data files
  FILE *organmeshfile;
  FILE *templatefile;
  FILE *adjointdatafile;
  FILE *seedmeshfile;

  //prescribed dose (Gy)
  double Dp = 145;
  
  //total decays assuming initial normalization to 1mCi
  double str = 0.4;
  //double norm = str*(3.7e10/1000)/(log(2)/(59.4*3600*24));
  
  //load data for optimization
  std::list<ratio> templateratios;
  std::list<ratio>::iterator rit;
  std::list<ratio> templateratios_reset;
  std::list<ratio> templateratios_copy;
  std::vector<double> adjoint_ratios;
  std::vector<int> urethra;
  std::vector<double> urethra_adjoint;
  int urethra_vol; //number of voxels
  std::vector<int> rectum;
  std::vector<double> rectum_adjoint;
  int rectum_vol; //number of voxels
  std::vector<int> normal;
  std::vector<double> normal_adjoint;
  int normal_vol; //number of voxels
  std::vector<int> target;
  std::vector<double> target_adjoint;
  int target_vol; //number of voxels
  std::vector<int> margin;
  std::vector<double> margin_adjoint;
  int margin_vol;
  std::vector<int> needle_template;
  int xneedlepts;
  int xmeshpts;
  int yneedlepts;
  int ymeshpts;
  int zneedlepts;
  int zmeshpts;
  std::vector<double> orgxpts;
  std::vector<double> orgypts;
  std::vector<double> orgzpts;
  int totS; //total seed positions
  int totN; //total needle positions
  double dxo;
  double dxn;
  double dxs;
  double dyo;
  double dyn;
  double dys;
  double dzo;
  double dzn;
  double dzs;
  int xseedpts;
  int xseedcent;
  int yseedpts;
  int yseedcent;
  int zseedpts;
  int zseedcent;
  std::vector<double> dosedata;

  //solution data
  std::list<ratio> solution;
  std::list<ratio> solution_copy;
  std::map<int,int> needles;
  std::vector<double> currentdose;
  std::vector<double> currentdose_copy;
  int numV100 = 0;

  //open necessary files
  if(argc != 5){
    printf("Please specify:\n\t1.) the organ mesh file\n\t2.) the template file\n\t3.) the adjoint data file\n\t4.) the seed mesh file.\n");
    exit(1);
  }
  else{
    organmeshfile = fopen(argv[1],"r");
    templatefile = fopen(argv[2],"r");
    adjointdatafile = fopen(argv[3],"r");
    seedmeshfile = fopen(argv[4],"r");
  }
  printf("reading in data from files\n");
  
  //-------------------READ IN DATA FROM FILES--------------------------------//
  char *dummy = (char*) malloc(256*sizeof(char));
  fscanf(organmeshfile,"%s %i %s %lf %s %i %s %lf %s %i %s %lf",
	 dummy,&xmeshpts,
	 dummy,&dxo,
	 dummy,&ymeshpts,
	 dummy,&dyo,
	 dummy,&zmeshpts,
	 dummy,&dzo);
  //printf("%i %lf %i %lf %i %lf\n",xmeshpts,dxo,ymeshpts,dyo,zmeshpts,dzo);
  fscanf(organmeshfile,"%s %i",dummy,&target_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    fscanf(organmeshfile,"%i",&tmp);
    target.push_back(tmp);
  }
  fscanf(organmeshfile,"%s %i",dummy,&urethra_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    fscanf(organmeshfile,"%i",&tmp);
    urethra.push_back(tmp);
  }
  fscanf(organmeshfile,"%s %i",dummy,&rectum_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    fscanf(organmeshfile,"%i",&tmp);
    rectum.push_back(tmp);
  }
  fscanf(organmeshfile,"%s %i",dummy,&normal_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    fscanf(organmeshfile,"%i",&tmp);
    normal.push_back(tmp);
  }
  fscanf(organmeshfile,"%s %i",dummy,&margin_vol);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    fscanf(organmeshfile,"%i",&tmp);
    margin.push_back(tmp);
  }
  fclose(organmeshfile);

  //read in template data
  fscanf(templatefile,"%s %s %s %s %s %s %s %s %s %s %s %s",
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    int tmp;
    fscanf(templatefile,"%i",&tmp);
    needle_template.push_back(tmp);
  }
  fclose(templatefile);
  
  //read in adjoint data
  fscanf(adjointdatafile,"%s %s %s %s %s %s %s %s %s %s %s %s",
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy,
	 dummy, dummy);
  fscanf(adjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    fscanf(adjointdatafile,"%lf",&tmp);
    target_adjoint.push_back(tmp);
  }
  fscanf(adjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    fscanf(adjointdatafile,"%lf",&tmp);
    urethra_adjoint.push_back(tmp);
  }
  fscanf(adjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    fscanf(adjointdatafile,"%lf",&tmp);
    rectum_adjoint.push_back(tmp);
  }
  fscanf(adjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    fscanf(adjointdatafile,"%lf",&tmp);
    normal_adjoint.push_back(tmp);
  }
  fscanf(adjointdatafile,"%s %s",dummy,dummy);
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    double tmp;
    fscanf(adjointdatafile,"%lf",&tmp);
    margin_adjoint.push_back(tmp);
  }
  fclose(adjointdatafile);

  //read in seed dose distribution data from seed mesh file
  fscanf(seedmeshfile,"%s %i %s %i %s %lf %s %i %s %i %s %lf %s %i %s %i %s %lf",     dummy, &xseedpts,
	 dummy, &xseedcent,
	 dummy, &dxs,
	 dummy, &yseedpts,
	 dummy, &yseedcent,
	 dummy, &dys,
	 dummy, &zseedpts,
	 dummy, &zseedcent,
	 dummy, &dzs);
  //algorithm assumes that the seed mesh elements and the organ mesh elements
  // are identical - if not, quit
  if(dxs != dyo || dys != dyo || dzs != dzo){
    printf("error: seed mesh elements are different than organ mesh elements\n");
    exit(1);
  }
  for(int i=0; i<xseedpts*yseedpts*zseedpts; i++){
    double tmp;
    fscanf(seedmeshfile,"%lf",&tmp);
    dosedata.push_back(tmp*str);
  }
  fclose(seedmeshfile);
  
  free(dummy);
  
  //-------------------START TREATMENT PLANNING ALGORITHM---------------------//
  printf("file reading done - starting treatment planning\n");
  
  //limit potential seed positions to those that are in the target
  //transform(needle_template.begin(), needle_template.end(), target.begin(),
  //	    needle_template.begin(), op_int_mult);
  
  //create the adjoint ratios along the template positions
  adjoint_ratios.resize(target_adjoint.size());
  for(int i=0; i<xmeshpts*ymeshpts*zmeshpts; i++){
    adjoint_ratios[i] = (urethra_adjoint[i]+rectum_adjoint[i]+
			 margin_adjoint[i])/target_adjoint[i];
    //adjoint_ratios[i] = (urethra_adjoint[i]+rectum_adjoint[i]+
    //			 margin_adjoint[i]+normal_adjoint[i])/target_adjoint[i];
  }
  ratio tmpratio;
  for(int k=0; k<zmeshpts; k++){
    for(int j=4; j<ymeshpts; j+=5){
      for(int i=4; i<xmeshpts; i+=5){
	if(needle_template[i+xmeshpts*j+xmeshpts*ymeshpts*k] == 1){
	  tmpratio.xindex = i;
	  tmpratio.yindex = j;
	  tmpratio.zindex = k;
	  tmpratio.value = adjoint_ratios[i+xmeshpts*j+xmeshpts*ymeshpts*k];
	  templateratios.push_back(tmpratio);
	}
      }
    }
  }

  //sort the template ratios
  templateratios.sort();
  rit = templateratios.begin();
  
  templateratios_reset = templateratios;
  
  //clear unneeded adjoint data
  adjoint_ratios.clear();
  target_adjoint.clear();
  urethra_adjoint.clear();
  rectum_adjoint.clear();
  normal_adjoint.clear();
  margin_adjoint.clear();
  
  //determine the minimum number of needles that will be needed
  // Sua's linear fit (based on target volume)
  int minN = (int)floor(0.24*(target_vol*0.1*0.1*0.5) + 11.33);

  //determine min seed isodose constant for seed selection process
  double Ciso1max = std::numeric_limits<double>::infinity();
  double Ciso1tmp = 0.0;
  double dosetmp;
  std::vector<double> target_dose_tmp;
  target_dose_tmp.resize(target.size());
  for(int k=0, Ik=zseedcent-k, Fk=Ik+zmeshpts; k<zmeshpts; k++, Ik-=1, Fk-=1){
    for(int j=4, Ij=yseedcent-j, Fj=Ij+ymeshpts; j<ymeshpts; j+=5, Ij-=5, Fj-=5){
      for(int i=4, Ii=xseedcent-i, Fi=Ii+xmeshpts; i<xmeshpts; i+=5, Ii-=5, Fi-=5){
	
	if(needle_template[i+xmeshpts*j+xmeshpts*ymeshpts*k] == 1){
	  //need a way to "vectorize" this assignment operation
	  for(int n=Ik, kl=0; n<Fk; n++, kl++){
	    for(int m=Ij, jl=0; m<Fj; m++, jl++){
	      for(int l=Ii, il=0; l<Fi; l++, il++){
		if(target[il+xmeshpts*jl+xmeshpts*ymeshpts*kl]==1){
		  dosetmp = dosedata[l+xseedpts*m+xseedpts*yseedpts*n];
		  if(dosetmp > Dp) dosetmp = Dp;
		  Ciso1tmp += dosetmp;
		}
	      }
	    }
	  }
	  Ciso1tmp /= (target_vol*Dp);
	  if(Ciso1tmp < Ciso1max)
	    Ciso1max = Ciso1tmp;
	  Ciso1tmp = 0.0;
	}
      }
    }
  }
  target_dose_tmp.clear();
  
  double Ciso2start;

  //number of needles chosen
  int numN = 0;

  //number of seeds chosen
  int numS = 0;
  int numS_copy = 0;

  //current seed index
  int Si, Sj, Sk;  

  //resize current dose vector
  currentdose.resize(target.size());

  //start selection process
  for(int goalN = minN; goalN <= 19; goalN++){
    //printf("current goalN: %i\n",goalN);
    for(double Ciso1 = 0.001; Ciso1 <= Ciso1max; Ciso1 += 0.001){
	     
      //set current dose for each voxel to zero
      transform(currentdose.begin(),currentdose.end(),currentdose.begin(),
		op_double_zero);
      
      //reset needle map
      needles.clear();
      
      //select first seed
      solution.push_back(templateratios.front());
      templateratios.pop_front();
      Si = solution.back().xindex;
      Sj = solution.back().yindex;
      Sk = solution.back().zindex;
      numN++;
      numS++;
      
      //assign needle index to needle map
      needles[Si + xmeshpts*Sj] = 1;
      
      //map dose from first seed
      numV100 = 0;
      //need a way to "vectorize" this assignment operation
      for(int n=zseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
	for(int m=yseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
	  for(int l=xseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
	    currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] = 
	      dosedata[l+xseedpts*m+xseedpts*yseedpts*n];
	  }
	}
      }
      
      //keep selecting seeds until the needle goal has been reached
      while(numN < goalN){
	for(rit = templateratios.begin(); rit != templateratios.end(); rit++){
	  Si = (*rit).xindex;
	  Sj = (*rit).yindex;
	  Sk = (*rit).zindex;
	  if(currentdose[Si+xmeshpts*Sj+xmeshpts*ymeshpts*Sk] < Dp*Ciso1*numS){
	    solution.push_back((*rit));
	    rit = templateratios.erase(rit);
	    numS++;
	    if(needles.count(Si+xmeshpts*Sj) != 1){
	      needles[Si+xmeshpts*Sj] = numN+1;
	      numN++;
	    }
	    break;
	  }
	}
	if(rit == templateratios.end())
	  break;
	else{
	  //map dose from latest seed
	  numV100 = 0;
	  //need a way to "vectorize" this assignment operation
	  for(int n=zseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
	    for(int m=yseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
	      for(int l=xseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
		currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] += 
		  dosedata[l+xseedpts*m+xseedpts*yseedpts*n];
		if(currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl]*
		   target[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] >= Dp)
		  numV100++;
	      }
	    }
	  }
	}
      }
      if(numN != goalN){ //try a new Ciso1
	transform(currentdose.begin(),currentdose.end(),currentdose.begin(),
		  op_double_zero);
	needles.clear();
	numV100 = 0;
	numN = 0;
	numS = 0;
	templateratios = templateratios_reset;
	solution.clear();
	continue;
      }
      
      templateratios_copy = templateratios; //copies needed for reseting
      currentdose_copy = currentdose;
      solution_copy = solution;
      numS_copy = numS;
      //the rest of the seeds can only be selected along a current needle
      for(double Ciso2 = 1.02; Ciso2<=1.08; Ciso2 += 0.02){
	//continue selecting seeds until 98% of target volume receives Dp
	while((double)numV100/target_vol < 0.98){
	  for(rit = templateratios.begin(); rit != templateratios.end(); rit++){
	    Si = (*rit).xindex;
	    Sj = (*rit).yindex;
	    Sk = (*rit).zindex;
	    if(currentdose[Si+xmeshpts*Sj+xmeshpts*ymeshpts*Sk] < Dp*Ciso2 &&
	       needles.count(Si+xmeshpts*Sj) == 1){
	      solution.push_back((*rit));
	      rit = templateratios.erase(rit);
	      numS++;
	      break;
	    }
	  }
	  if(rit == templateratios.end())
	    break; //bad Ciso2
	  else{
	    //map dose from latest seed
	    numV100 = 0;
	    //need a way to "vectorize" this assignment operation
	    for(int n=zseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
	      for(int m=yseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
		for(int l=xseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
		  currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] += 
		    dosedata[l+xseedpts*m+xseedpts*yseedpts*n];
		  if(currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl]*
		     target[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] >= Dp)
		    numV100++;
		}
	      }
	    }
	  }
	}
	if((double)numV100/target_vol < 0.98){ //Ciso2 not successful
	  templateratios = templateratios_copy;
	  currentdose = currentdose_copy;
	  solution = solution_copy;
	  numS= numS_copy;
	  numV100 = 0;
	}
	else if((double)numV100/target_vol >= 0.98){
	  Ciso2start = Ciso2;
	  break;
	}
      }
      
      if((double)numV100/target_vol < 0.98){ //bad Ciso1
	transform(currentdose.begin(),currentdose.end(),currentdose.begin(),
		  op_double_zero);
	needles.clear();
	numV100 = 0;
	numN = 0;
	numS = 0;
	templateratios = templateratios_reset;
	solution.clear();
	continue;
      }
      else if((double)numV100/target_vol >= 0.98)
	printf("Ciso1: %lf\n",Ciso1);
	break;
    }
    
    if((double)numV100/target_vol >= 0.98){
      //check for a more precise Ciso2
      templateratios = templateratios_copy;
      currentdose = currentdose_copy;
      solution = solution_copy;
      numS = numS_copy;
      numV100 = 0;
      for(double Ciso2 = Ciso2start-0.019; Ciso2 <= Ciso2start; Ciso2 += 0.001){
	//continue selecting seeds until 98% of target volume receives Dp
	while((double)numV100/target_vol < 0.98){
	  for(rit = templateratios.begin(); rit != templateratios.end(); rit++){
	    Si = (*rit).xindex;
	    Sj = (*rit).yindex;
	    Sk = (*rit).zindex;
	    if(currentdose[Si+xmeshpts*Sj+xmeshpts*ymeshpts*Sk] < Dp*Ciso2 &&
	       needles.count(Si+xmeshpts*Sj) == 1){
	      solution.push_back((*rit));
	      rit = templateratios.erase(rit);
	      numS++;
	      break;
	    }
	  }
	  if(rit == templateratios.end())
	    break; //bad Ciso2
	  else{
	    //map dose from latest seed
	    numV100 = 0;
	    //need a way to "vectorize" this assignment operation
	    for(int n=zseedcent-Sk, Fk=n+zmeshpts, kl=0; n<Fk; n++, kl++){
	      for(int m=yseedcent-Sj, Fj=m+ymeshpts, jl=0; m<Fj; m++, jl++){
		for(int l=xseedcent-Si, Fi=l+xmeshpts, il=0; l<Fi; l++, il++){
		  currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] += 
		    dosedata[l+xseedpts*m+xseedpts*yseedpts*n];
		  if(currentdose[il+xmeshpts*jl+xmeshpts*ymeshpts*kl]*
		     target[il+xmeshpts*jl+xmeshpts*ymeshpts*kl] >= Dp)
		    numV100++;
		}
	      }
	    }
	  }
	}
	if((double)numV100/target_vol < 0.98){ //Ciso2 not successful
	  templateratios = templateratios_copy;
	  currentdose = currentdose_copy;
	  solution = solution_copy;
	  numS= numS_copy;
	  numV100 = 0;
	}
	else if((double)numV100/target_vol >= 0.98){
	  printf("Ciso2: %lf\n",Ciso2);
	  break;
	}
      }
      
      if((double)numV100/target_vol >= 0.98){
	printf("needle goal: %i\n",goalN);
	printf("numV100: %lf\n", (double)(numV100)/target_vol);
	break;
      }
    }
  }
  clock_t end = clock();

  //print treatment plan
  printf("numN\tnumS\tSi\tSj\tSk\n");
  int seednum;
  int sx,sy,sz;
  for(rit = solution.begin(), seednum=1; rit != solution.end();rit++,seednum++){
    sx = (*rit).xindex;
    sy = (*rit).yindex;
    sz = (*rit).zindex;
    printf("%i\t%i\t%i\t%i\t%i\n",needles.find(sx+xmeshpts*sy)->second, 
	   seednum, sx, sy, sz);
      
  }
  printf("Plan created in %lf sec\n",1.0*(end-start)/CLOCKS_PER_SEC);
  
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

  return 0;
}

