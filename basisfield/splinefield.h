// Declarations of class splinefield
//
// splinefield.h
//
// Jesper Andersson, FMRIB Image Analysis Group
//
// Copyright (C) 2007 University of Oxford
//
//     CCOPYRIGHT
//

#ifndef splinefield_h
#define splinefield_h

#include <string>
#include <vector>
#include <memory>
#include "armawrap/newmat.h"
#include "utils/threading.h"
#include "miscmaths/bfmatrix.h"
#include "fsl_splines.h"
#include "basisfield.h"

namespace BASISFIELD {

// Declare ahead
class Memen_H_Helper;
class helper_vol;

enum EnergyType {MemEn, BendEn};
enum HtHbType {HtHb, Hb};

//
// This is a helper-class that keeps a map of all spline coefficients
// and keeps track of the coefficients/splines for which their entire
// support in image space is zero. That means that the cross-procuct
// of that spline with any other spline will be zero and will not need
// to be calculated.
//
class ZeroSplineMap
{
public:
  template<class T, class S>
  ZeroSplineMap(const Spline3D<T>&                sp,
                const std::vector<unsigned int>&  csz,
                const S                           *ima,
                const std::vector<unsigned int>&  isz);
  ~ZeroSplineMap() {delete[] _map;}

  // Check for zero without range check
  bool IsZero(const std::vector<unsigned int>&  cindx) const {return static_cast<bool>(_map[(cindx[2]*_csz[1]+cindx[1])*_csz[0]+cindx[0]]);}
  // Check for zero with range/error check
  bool operator()(const std::vector<unsigned int>& cindx) const {
    if (cindx.size() != 3) throw BasisfieldException("ZeroSplineMap::operator(): Index vector must be of size 3");
    if (cindx[0] >= _csz[0] || cindx[1] >= _csz[1] || cindx[2] >= _csz[2]) throw BasisfieldException("ZeroSplineMap::operator(): Index out of range");
    return(IsZero(cindx));
  }
private:
  template<class S>
  char all_zeros(const std::vector<unsigned int>&  first,
                 const std::vector<unsigned int>&  last,
                 const S                           *ima,
                 const std::vector<unsigned int>&  isz) const;

  std::vector<unsigned int>  _csz;
  char                       *_map;
};

class splinefield: public basisfield
{
public:

  // Constructors and destructors, including assignment

  // Target constructor
  splinefield(const std::vector<unsigned int>& psz,
	      const std::vector<double>&       pvxs,
	      const std::vector<unsigned int>& pksp,
	      int                              porder,
	      Utilities::NoOfThreads           nthr);
  // Delegating constructors
  splinefield(const std::vector<unsigned int>& psz,
	      const std::vector<double>&       pvxs,
	      const std::vector<unsigned int>& pksp,
	      int                              porder) : splinefield(psz,pvxs,pksp,porder,Utilities::NoOfThreads(1)) {}
  splinefield(const std::vector<unsigned int>& psz,
	      const std::vector<double>&       pvxs,
	      const std::vector<unsigned int>& pksp) : splinefield(psz,pvxs,pksp,3,Utilities::NoOfThreads(1)) {}

  // Copy constructor
  splinefield(const splinefield& inf);
  splinefield& operator=(const splinefield& inf);
  virtual ~splinefield() {} // This should drop straight through to base-class

  // Explicit instruction to compiler that we intend not to refine some Peek functions
  using basisfield::Peek;
  // Getting the value for a non-integer voxel location
  virtual double Peek(double x, double y, double z, FieldIndex fi=FIELD) const;
  virtual double Peek(float x, float y, float z, FieldIndex fi=FIELD) const {return(Peek(static_cast<double>(x),static_cast<double>(y),static_cast<double>(z)));}

  // General utility functions

  virtual unsigned int CoefSz_x() const {
    // cout << "Old value would have been " << static_cast<unsigned int>(ceil(((double(FieldSz_x())) + 1.0) / (double(Ksp_x())))) + 2 << endl;
    // cout << "New value is " << _sp.NCoef(0,FieldSz_x()) << endl;
    return(_sp.NCoef(0,FieldSz_x()));
  }
  virtual unsigned int CoefSz_y() const {
    // cout << "Old value would have been " << static_cast<unsigned int>(ceil(((double(FieldSz_y())) + 1.0) / (double(Ksp_y())))) + 2 << endl;
    // cout << "New value is " << _sp.NCoef(1,FieldSz_y()) << endl;
    if (NDim()<2) return(1);
    else return(_sp.NCoef(1,FieldSz_y()));
  }
  virtual unsigned int CoefSz_z() const {
    // cout << "Old value would have been " << static_cast<unsigned int>(ceil(((double(FieldSz_z())) + 1.0) / (double(Ksp_z())))) + 2 << endl;
    // cout << "New value is " << _sp.NCoef(2,FieldSz_z()) << endl;
    if (NDim()<3) return(1);
    else return(_sp.NCoef(2,FieldSz_z()));
  }

  virtual bool HasGlobalSupport() const {return(false);}

  virtual unsigned int Nthreads() const {return(_nthr);}
  virtual void SetNthreads(unsigned int nthr) {_nthr=nthr;}

  virtual unsigned int Order() const {return(_sp.Order());}
  virtual unsigned int Ksp_x() const {return(_sp.KnotSpacing(0));}
  virtual unsigned int Ksp_y() const {return(_sp.KnotSpacing(1));}
  virtual unsigned int Ksp_z() const {return(_sp.KnotSpacing(2));}
  virtual unsigned int KernelSz_x() const {return(_sp.KernelSize(0));}
  virtual unsigned int KernelSz_y() const {return(_sp.KernelSize(1));}
  virtual unsigned int KernelSz_z() const {return(_sp.KernelSize(2));}

  // Functions that actually do some work

  using basisfield::Set; // Instruct compiler that we intentionally don't refine all Set functions
  virtual void Set(const NEWIMAGE::volume<float>& pfield);

  virtual void SetToConstant(double fv);

  // Get the range of basis-functions with support at point xyz
  virtual void RangeOfBasesWithSupportAtXyz(const NEWMAT::ColumnVector&       xyz,
                                            std::vector<unsigned int>&        first,
                                            std::vector<unsigned int>&        last) const
  {
    std::vector<double>  lxyz(3);
    lxyz[0] = static_cast<double>(xyz(1)); lxyz[1] = static_cast<double>(xyz(2)); lxyz[2] = static_cast<double>(xyz(3));
    std::vector<unsigned int>  coefsz(3);
    coefsz[0] = CoefSz_x(); coefsz[1] = CoefSz_y(); coefsz[2] = CoefSz_z();
    _sp.RangeOfSplines(lxyz,coefsz,first,last);
  }

  // Get the value of basis lmn at point xyz
  virtual double ValueOfBasisLmnAtXyz(const std::vector<unsigned int>&  lmn,
                                      const NEWMAT::ColumnVector&       xyz) const
  {
    std::vector<double>  lxyz(3);
    lxyz[0] = static_cast<double>(xyz(1)); lxyz[1] = static_cast<double>(xyz(2)); lxyz[2] = static_cast<double>(xyz(3));
    return(_sp.SplineValueAtVoxel(lxyz,lmn));
  }

  virtual std::vector<double> SubsampledVoxelSize(unsigned int               ss,
			                          std::vector<double>        vxs = std::vector<double>(),
				                  std::vector<unsigned int>  ms = std::vector<unsigned int>()) const
  {
    std::vector<unsigned int>  ssv(NDim(),ss);
    return(SubsampledVoxelSize(ssv,vxs,ms));
  }
  virtual std::vector<double> SubsampledVoxelSize(const std::vector<unsigned int>&  ss,
			                          std::vector<double>               vxs = std::vector<double>(),
				                  std::vector<unsigned int>         ms = std::vector<unsigned int>()) const;

  virtual std::vector<unsigned int> SubsampledMatrixSize(unsigned int               ss,
                                                         std::vector<unsigned int>  ms = std::vector<unsigned int>()) const
  {
    std::vector<unsigned int>  ssv(NDim(),ss);
    return(SubsampledMatrixSize(ssv,ms));
  }
  virtual std::vector<unsigned int> SubsampledMatrixSize(const std::vector<unsigned int>&  ss,
                                                         std::vector<unsigned int>         ms = std::vector<unsigned int>()) const;

  virtual void Update(FieldIndex fi);

  virtual void Update(const std::vector<FieldIndex>& fiv);

  virtual NEWMAT::ReturnMatrix Jte(const NEWIMAGE::volume<float>&  ima1,
                                   const NEWIMAGE::volume<float>&  ima2,
                                   const NEWIMAGE::volume<char>    *mask) const;

  virtual NEWMAT::ReturnMatrix Jte(const std::vector<unsigned int>&  deriv,
                                   const NEWIMAGE::volume<float>&    ima1,
                                   const NEWIMAGE::volume<float>&    ima2,
                                   const NEWIMAGE::volume<char>      *mask) const;

  virtual NEWMAT::ReturnMatrix Jte(const NEWIMAGE::volume<float>&    ima,
                                   const NEWIMAGE::volume<char>      *mask) const;

  virtual NEWMAT::ReturnMatrix Jte(const std::vector<unsigned int>&  deriv,
                                   const NEWIMAGE::volume<float>&    ima,
                                   const NEWIMAGE::volume<char>      *mask) const;

  virtual std::shared_ptr<MISCMATHS::SpMat<float> > J() const;

  virtual std::shared_ptr<MISCMATHS::SpMat<float> > J(const std::vector<unsigned int>&  deriv) const;

  virtual std::shared_ptr<MISCMATHS::SpMat<float> > J(const NEWIMAGE::volume<float>&    ima,
						      const NEWIMAGE::volume<char>      *mask) const;

  virtual std::shared_ptr<MISCMATHS::SpMat<float> > J(const std::vector<unsigned int>&  deriv,
						      const NEWIMAGE::volume<float>&    ima,
						      const NEWIMAGE::volume<char>      *mask) const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> JtJ(const NEWIMAGE::volume<float>&    ima,
                                                   const NEWIMAGE::volume<char>      *mask,
                                                   MISCMATHS::BFMatrixPrecisionType  prec) const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> JtJ(const NEWIMAGE::volume<float>&       ima1,
                                                   const NEWIMAGE::volume<float>&       ima2,
                                                   const NEWIMAGE::volume<char>         *mask,
                                                   MISCMATHS::BFMatrixPrecisionType     prec) const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> JtJ(const std::vector<unsigned int>&       deriv,
                                                   const NEWIMAGE::volume<float>&         ima,
                                                   const NEWIMAGE::volume<char>           *mask,
                                                   MISCMATHS::BFMatrixPrecisionType       prec) const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> JtJ(const std::vector<unsigned int>&       deriv,
                                                   const NEWIMAGE::volume<float>&         ima1,
                                                   const NEWIMAGE::volume<float>&         ima2,
                                                   const NEWIMAGE::volume<char>           *mask,
                                                   MISCMATHS::BFMatrixPrecisionType       prec) const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> JtJ(const std::vector<unsigned int>&         deriv1,
                                                   const NEWIMAGE::volume<float>&           ima1,
                                                   const std::vector<unsigned int>&         deriv2,
                                                   const NEWIMAGE::volume<float>&           ima2,
                                                   const NEWIMAGE::volume<char>             *mask,
                                                   MISCMATHS::BFMatrixPrecisionType         prec) const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> JtJ(const NEWIMAGE::volume<float>&        ima1,
                                                   const basisfield&                     bf2,
                                                   const NEWIMAGE::volume<float>&        ima2,
                                                   const NEWIMAGE::volume<char>          *mask,
                                                   MISCMATHS::BFMatrixPrecisionType      prec) const;


  virtual double MemEnergy() const;
  virtual double BendEnergy() const;

  virtual NEWMAT::ReturnMatrix MemEnergyGrad() const;
  virtual NEWMAT::ReturnMatrix BendEnergyGrad() const;

  virtual std::shared_ptr<MISCMATHS::BFMatrix> MemEnergyHess(MISCMATHS::BFMatrixPrecisionType   prec) const;
  virtual std::shared_ptr<MISCMATHS::BFMatrix> BendEnergyHess(MISCMATHS::BFMatrixPrecisionType   prec) const;
  virtual std::shared_ptr<MISCMATHS::SpMat<float> > BendEnergyHessAsSpMat() const;

  virtual std::shared_ptr<BASISFIELD::basisfield> ZoomField(const std::vector<unsigned int>&    psz,
                                                              const std::vector<double>&          pvxs,
                                                              std::vector<unsigned int>           pksp=std::vector<unsigned int>()) const;

private:
  Spline3D<double>                                 _sp;          // Spline that we can ask questions of
  std::vector<std::shared_ptr<Spline3D<double> > > _dsp;         // Derivatives
  unsigned int                                     _nthr;        // If !=1 means to run multi-threaded version
  mutable std::mutex                               _print_mutex; // Use by threads to write to screen

  // Functions for internal use

  double subsampled_voxel_size(unsigned int   ss,           // Subsampling factor
                               double         ovxs) const;  // Old voxel size
  unsigned int subsampled_matrix_size(unsigned int  ss,         // Subsampling factor
                                      unsigned int  oms) const; // Old matrix size
  double *get_spline_kernel(const std::vector<unsigned int>& deriv, int *ksz) const;
  std::vector<unsigned int> next_size_down(const std::vector<unsigned int>& isize) const;
  unsigned int next_size_down(unsigned int isize) const;
  bool is_a_power_of_2(double fac) const;
  bool is_a_power_of_2(unsigned int fac) const;
  bool are_a_power_of_2(const std::vector<double>& facs) const;
  bool are_a_power_of_2(const std::vector<unsigned int>& facs) const;
  bool new_vxs_is_ok(const std::vector<double>& nvxs,
                     std::vector<double>        ovxs=std::vector<double>()) const;
  bool new_vxs_is_ok(double nvxs, double ovxs) const;
  std::vector<unsigned int> fake_old_ksp(const std::vector<double>&        nvxs,
                                         const std::vector<unsigned int>&  nksp,
                                         std::vector<double>               ovxs=std::vector<double>()) const;
  unsigned int fake_old_ksp(double nvxs, unsigned int nksp, double ovxs) const;

  bool faking_works(const std::vector<double>&        nvxs,
                    const std::vector<unsigned int>&  nksp,
                    std::vector<double>               ovxs=std::vector<double>()) const;

  bool faking_works(double nvxs, unsigned int nksp, double ovxs) const;

  std::shared_ptr<BASISFIELD::basisfield> zoom_field_in_stupid_way(const std::vector<unsigned int>&    nms,
                                                                     const std::vector<double>&          nvxs,
                                                                     const std::vector<unsigned int>&    nksp) const;

  // Perform spline deconvolution along one dimension
  helper_vol get_coefs_one_dim(const helper_vol&                 in,
                               unsigned int                      dim,
                               unsigned int                      csz,
                               unsigned int                      order,
                               unsigned int                      ksp) const;

  // Regularisation matrix to avoid blowing up end coefficients
  NEWMAT::ReturnMatrix get_s_matrix(unsigned int isz,
                                    unsigned int csz) const;

  // Calculate a single field
  void get_field(const Spline3D<double>&            sp,
                 const double                       *c,
                 const std::vector<unsigned int>&   csz,
                 const std::vector<unsigned int>&   fsz,
                 double                             *f) const;

  // Calculate one or more fields
  void get_fields(const std::vector<Spline3D<double> >&  spv,
		  const double                           *coef,
		  const std::vector<unsigned int>&       csz,
		  const std::vector<unsigned int>&       fsz,
		  std::vector<double *>                  fldv) const;

  template<class T>
  void get_jte(const Spline3D<double>&            sp,
               const std::vector<unsigned int>&   csz,
               const T                            *ima,
               const std::vector<unsigned int>&   isz,
               double                             *jte) const;

  template<class T>
  void get_jte_para(const Spline3D<double>&            sp,          // Spline
		    const std::vector<unsigned int>&   csz,         // Size of Coefficient matrix for csp
		    const T                            *ima,        // Image
		    const std::vector<unsigned int>&   isz,         // Image size
		    double                             *jte,        // Jte
		    unsigned int                       nthr) const; // Number of threads

  template <class T>
  void calculate_jte_elements(// Input
			      unsigned int                      first_ci,    // First column index
			      unsigned int                      last_ci,     // One past last column index
			      const Spline3D<double>&           sp,          // Spline
			      const std::vector<unsigned int>&  csz,         // Size of Coefficient matrix for csp
			      const T                           *ima,        // Image
			      const std::vector<unsigned int>&  isz,         // Image size
			      // Output
			      double                            *jte) const; // Well, Jte.

  std::shared_ptr<MISCMATHS::SpMat<float> > get_j(const Spline3D<double>&            sp,
						    const std::vector<unsigned int>&   csz,
						    const float                        *ima,
						    const std::vector<unsigned int>&   isz) const;

  template<class T>
  std::shared_ptr<MISCMATHS::BFMatrix> make_asymmetric_jtj(const Spline3D<double>&           rsp,         // Spline that determines row in JtJ
                                                             const std::vector<unsigned int>&  cszr,        // Coefficient matrix size for rsp
                                                             const Spline3D<double>&           sp2,         // Spline that determines column in JtJ
                                                             const std::vector<unsigned int>&  cszc,        // Coefficient matrix size for csp/sp2
                                                             const T                           *ima,        // Image (typically product of two images)
                                                             const std::vector<unsigned int>&  isz,         // Matrix size of image
                                                             MISCMATHS::BFMatrixPrecisionType  prec) const; // double or float

  // Parallel version of function above
  template<class T>
  std::shared_ptr<MISCMATHS::BFMatrix> make_asymmetric_jtj_para(const Spline3D<double>&           rsp,   // Spline that determines row in JtJ
                                                                const std::vector<unsigned int>&  cszr,  // Coefficient matrix size for rsp
                                                                const Spline3D<double>&           csp,   // Spline that determines column in JtJ
                                                                const std::vector<unsigned int>&  cszc,  // Coefficient matrix size for csp/sp2
                                                                const T                           *ima,  // Image (typically product of two images)
                                                                const std::vector<unsigned int>&  isz,   // Matrix size of image
                                                                MISCMATHS::BFMatrixPrecisionType  prec,  // Precision (float/double) of resulting matrix
                                                                unsigned int                      nthr)  const; // Number of threads

  // Helper function for make_asymmetric_jtj_para
  void no_of_non_zero_elements_asym(// Input
				    unsigned int  first_ci,                // First column index
				    unsigned int  last_ci,                 // One past last column index
				    const std::vector<unsigned int>&  csz, // Size of Coefficient matrix for csp
				    const std::vector<unsigned int>&  isz, // Image size
				    const Spline3D<double>&           csp, // Spline that determines column in JtJ
				    const Spline3D<double>&           rsp, // Spline that determines row in JtJ
				    // Output
				    unsigned int& nnz) const;              // Total no. of non-zero elements in columns first_ci--last_ci-1

  // Helper function for make_asymmetric_jtj_para
  template<class T>
  void calculate_non_zero_elements_asym(// Input
					unsigned int                      first_ci,     // First column index
					unsigned int                      last_ci,      // One past last column index
					unsigned int                      vali,         // First index into irp and valp
					const std::vector<unsigned int>&  cszc,         // Size of coefficient matrix for "column splines"
					const std::vector<unsigned int>&  cszr,         // Size of coefficient matrix for "row splines"
					const std::vector<unsigned int>&  isz,          // Size of image matrix
					Spline3D<double>                  csp,          // Spline defining columns
					const Spline3D<double>&           rsp,          // Spline defining rows
					bool                              sks,          // True if csp and rsp has same size
					const ZeroSplineMap&              rzm,          // Keeps track of splines for which ima is zero over entire support
					const ZeroSplineMap&              czm,          // Keeps track of splines for which ima is zero over entire support
					const T                           *ima,         // Image
					// Output
					unsigned int                      *irp,         // nzmax length array of row indicies
					unsigned int                      *jcp,         // n+1 length array of starts of column indicies
					double                            *valp) const; // nzmax length array of values

  template<class T>
  std::shared_ptr<MISCMATHS::BFMatrix> make_fully_symmetric_jtj(const Spline3D<double>&            sp2,
                                                                  const std::vector<unsigned int>&   csz,
                                                                  const T                            *ima,
                                                                  const std::vector<unsigned int>&   isz,
                                                                  MISCMATHS::BFMatrixPrecisionType   prec) const;

  // Helper function for make_fully_symmetric_jtj
  double get_val(unsigned int           row,          // The row we want to find the value of
                 unsigned int           col,          // The column we want to find the value of
                 const unsigned int     *irp,         // Array of row-indicies
                 const unsigned int     *jcp,         // Array of indicies into irp
                 const double           *val) const;  // Array of values sorted as irp

  // Parallel version of make_fully_symmetric_jtj above
  template<class T>
  std::shared_ptr<MISCMATHS::BFMatrix> make_fully_symmetric_jtj_para(const Spline3D<double>&            sp2,
                                                                     const std::vector<unsigned int>&   csz,
                                                                     const T                            *ima,
                                                                     const std::vector<unsigned int>&   isz,
                                                                     MISCMATHS::BFMatrixPrecisionType   prec,
                                                                     unsigned int                       nthr) const;

  // Helper function for make_fully_symmetric_jtj_para
  void no_of_non_zero_elements_sym(// Input
				   unsigned int  first_ci,                // First column index
				   unsigned int  last_ci,                 // One past last column index
				   const std::vector<unsigned int>&  csz, // Size of Coefficient matrix for csp
				   const std::vector<unsigned int>&  isz, // Image size
				   const Spline3D<double>&           sp,  // Spline that determines column/row in JtJ
				   // Output
				   unsigned int& nnz) const;              // Total no. of non-zero elements in columns first_ci--last_ci-1

  // Helper function for make_fully_symmetric_jtj_para
  template<class T>
  void calculate_non_zero_elements_sym(// Input
				       unsigned int                      first_ci,     // First column index
				       unsigned int                      last_ci,      // One past last column index
				       unsigned int                      vali,         // First index into irp and valp
				       const std::vector<unsigned int>&  csz,          // Size of coefficient matrix
				       const std::vector<unsigned int>&  isz,          // Size of image matrix
				       const Spline3D<double>&           rsp,          // Spline determining row in JtJ
				       const ZeroSplineMap&              zeromap,      // Keeps track of splines for which ima is zero over entire support
				       const T                           *ima,         // Image
				       // Output
				       unsigned int                      *irp,         // nzmax length array of row indicies
				       unsigned int                      *jcp,         // n+1 length array of starts of column indicies
				       double                            *valp) const; // nzmax length array of values

  // Helper function for make_fully_symmetric_jtj_para
  void fill_in_non_zero_elements_sym(// Input
				     unsigned int                      first_ci,     // First column index
				     unsigned int                      last_ci,      // One past last column index
				     const std::vector<unsigned int>&  csz,          // Size of coefficient matrix
				     const std::vector<unsigned int>&  isz,          // Size of image matrix
				     const Spline3D<double>&           sp,           // Spline
				     // Output
				     const unsigned int                *irp,         // nzmax length array of row indicies
				     const unsigned int                *jcp,         // n+1 length array of starts of column indicies
				     double                            *valp) const; // nzmax length array of values

  // Helper function for make_fully_symmetric_jtj_para
  void set_val(unsigned int           row,        // The row we want to find the value of
	       unsigned int           col,        // The column we want to find the value of
	       const unsigned int     *irp,       // Array of row-indicies
	       const unsigned int     *jcp,       // Array of indicies into irp
	       double                 *valp,      // Array of values sorted as irp
	       double                 val) const; // The value to set

  void calculate_memen_AtAb(const NEWMAT::ColumnVector&       b,
                            const std::vector<unsigned int>&  lksp,
                            const std::vector<unsigned int>&  isz,
                            const std::vector<double>&        vxs,
                            const std::vector<unsigned int>&  csz,
                            unsigned int                      sp_ord,
                            NEWMAT::ColumnVector&             grad) const;

  void calculate_bender_AtAb(const NEWMAT::ColumnVector&       b,
                             const std::vector<unsigned int>&  lksp,
                             const std::vector<unsigned int>&  isz,
                             const std::vector<double>&        vxs,
                             const std::vector<unsigned int>&  csz,
                             unsigned int                      sp_ord,
                             NEWMAT::ColumnVector&             grad) const;

  void calculate_AtAb(const NEWMAT::ColumnVector&       b,
                      const std::vector<unsigned int>&  isz,
                      const std::vector<unsigned int>&  csz,
                      const Spline3D<double>&           sp,
                      const Memen_H_Helper&             hlpr,
                      NEWMAT::ColumnVector&             grad) const;

  void calculate_memen_bender_H(// Input
				const std::vector<unsigned int>&  lksp,        // Knot-spacing
				const std::vector<unsigned int>&  csz,         // Size of coefficent grid
				const std::vector<unsigned int>&  isz,         // Image size
				EnergyType                        et,          // Membrane or bending energy
				// Output. References because they are allocated here.
				unsigned int*&                    irp,         // Row index vector
				unsigned int*&                    jcp,         // Vector with start indicies of columns
				double*&                          valp) const; // Values

  void calculate_memen_bender_H_para(// Input
				     const std::vector<unsigned int>&  lksp,        // Knot-spacing
				     const std::vector<unsigned int>&  csz,         // Size of coefficent grid
				     const std::vector<unsigned int>&  isz,         // Image size
				     EnergyType                        et,          // Membrane or bending energy
				     unsigned int                      nthr,        // No. of threads
				     // Output. References because they are allocated here.
				     unsigned int*&                    irp,         // Row index vector
				     unsigned int*&                    jcp,         // Vector with start indicies of columns
				     double*&                          valp) const; // Values

  void calculate_memen_bender_hess_no_of_elements(//Input
						  unsigned int                      first_ci,  // First column index
						  unsigned int                      last_ci,   // One past last column index
						  const std::vector<unsigned int>&  csz,       // Size of coefficient grid
						  const std::vector<unsigned int>&  isz,       // Size of image
						  const Spline3D<double>&           sp,        // Spline
						  // Output
						  unsigned int&                     nnz) const;// Number of non-zero elements

  void calculate_memen_bender_hess_columns(// Input
                                           unsigned int                      first_ci,     // First column index
                                           unsigned int                      last_ci,      // One past last column index
                                           unsigned int                      vali,         // Starting index into irp and valp
                                           const std::vector<unsigned int>&  csz,          // Size of coefficient grid
                                           const std::vector<unsigned int>&  isz,          // Size of image
                                           const Spline3D<double>&           sp,           // Spline
                                           unsigned int                      nh,           // Number of helpers
                                           std::shared_ptr<Memen_H_Helper>  *helpers,     // Array of helpers
                                           // Output
                                           unsigned int                     *irp,         // Row index vector
                                           unsigned int                     *jcp,         // Vector with start indicies of columns
                                           double                           *valp) const; // Values

  void hadamard(const NEWIMAGE::volume<float>& vol1,
                const NEWIMAGE::volume<float>& vol2,
                float                          *prod) const;
  void hadamard(const NEWIMAGE::volume<float>& vol1,
                const NEWIMAGE::volume<float>& vol2,
                const NEWIMAGE::volume<char>&  mask,
                float                          *prod) const;
  void hadamard(const NEWIMAGE::volume<float>& vol1,
                const NEWIMAGE::volume<float>& vol2,
                const NEWIMAGE::volume<char>   *mask,
                float                          *prod) const;

  void copy_and_mask_ima(const NEWIMAGE::volume<float>& ima,
			 const NEWIMAGE::volume<char>   *mask,
			 float                          *mima) const;


protected:
  // Functions for use in this and derived classes
  virtual void assign_splinefield(const splinefield& inf);
  virtual double peek_outside_fov(int i, int j, int k, FieldIndex fi) const;

};


/////////////////////////////////////////////////////////////////////
//
// This is a little helper class that stores all possible values
// of sum-of-products for a given spline for all possible overlaps.
//
/////////////////////////////////////////////////////////////////////

class Memen_H_Helper
{
public:
  Memen_H_Helper(const Spline3D<double>&  sp);
  Memen_H_Helper(const Memen_H_Helper& inh) : _sz(inh._sz), _cntr(inh._cntr) // Copy construction
  {
    _data = new double[_sz[0]*_sz[1]*_sz[2]];
    memcpy(_data,inh._data,_sz[0]*_sz[1]*_sz[2]*sizeof(double));
  }
  ~Memen_H_Helper() {delete [] _data;}
  unsigned int Size(unsigned int dir) const {return(static_cast<unsigned int>(_sz[dir]));}
  int LowerLimit(unsigned int dir) const {return(- _cntr[dir]);}
  int UpperLimit(unsigned int dir) const {return(_sz[dir] - _cntr[dir]);}
  double operator()(int i, int j, int k) const;  // Read access with range check
  double Peek(int i, int j, int k) const         // Read access without range check
  {
    return(_data[(k+_cntr[2])*_sz[1]*_sz[0] + (j+_cntr[1])*_sz[0] + (i+_cntr[0])]);
  }
  Memen_H_Helper& operator=(const Memen_H_Helper& inh)  // Assignment
  {
    if (this != &inh) {
      delete[] _data;
      _sz = inh._sz;
      _cntr = inh._cntr;
      _data = new double[_sz[0]*_sz[1]*_sz[2]];
      memcpy(_data,inh._data,_sz[0]*_sz[1]*_sz[2]*sizeof(double));
    }
    return(*this);
  }
  Memen_H_Helper& operator*=(double s)                  // Multiplication of self by scalar
  {
    unsigned int sz=_sz[2]*_sz[1]*_sz[0];
    for (unsigned int i=0; i<sz; i++) _data[i]*=s;
    return(*this);
  }
  Memen_H_Helper& operator+=(const Memen_H_Helper& rhs)         // Addition of other helper to self
  {
    if (_sz[0]!=rhs._sz[0] || _sz[1]!=rhs._sz[1] || _sz[2]!=rhs._sz[2]) throw BasisfieldException("Memen_H_Helper::+=(): Size mismatch");
    unsigned int sz=_sz[2]*_sz[1]*_sz[0];
    for (unsigned int i=0; i<sz; i++) _data[i]+=rhs._data[i];
    return(*this);
  }
private:
  std::vector<int>      _sz;
  std::vector<int>      _cntr;
  double                *_data;
};

/////////////////////////////////////////////////////////////////////
//
// This is a little helper class that is used when deconvolving
// a field to obtain the spline coefficients.
//
/////////////////////////////////////////////////////////////////////

class helper_vol
{
public:
  helper_vol(const std::vector<unsigned int>& sz);
  helper_vol(const std::vector<unsigned int>& sz,
             const NEWMAT::ColumnVector&      vec);
  helper_vol(const NEWIMAGE::volume<float>&   vol);
  helper_vol(const helper_vol& in);
  ~helper_vol() { delete [] _data; }
  helper_vol& operator=(const helper_vol& rhs);
  unsigned int Size(unsigned int dim) const { if (dim>2) throw BasisfieldException("helper_vol::Size: dim must be 0, 1 or 2"); return(_sz[dim]); }
  NEWMAT::ReturnMatrix AsNewmatVector() const;
  void GetColumn(unsigned int i, unsigned int j, unsigned int dim, double *col) const;
  void SetColumn(unsigned int i, unsigned int j, unsigned int dim, const double *col);
private:
  unsigned int _sz[3];
  double       *_data;

  double* start(unsigned int i, unsigned int j, unsigned int dim) const;
  const double* end(unsigned int i, unsigned int j, unsigned int dim) const;
  unsigned int step(unsigned int dim) const;
};
/////////////////////////////////////////////////////////////////////
//
// Templated member-functions for the ZeroSplineMap class. The
// class will keep track of splines for which the/an image is zero
// for all of their support, thereby avoiding unneccessary calculations.
//
/////////////////////////////////////////////////////////////////////

template<class T, class S>
ZeroSplineMap::ZeroSplineMap(const Spline3D<T>&                sp,
                             const std::vector<unsigned int>&  csz,
                             const S                           *ima,
                             const std::vector<unsigned int>&  isz)
{
  if (csz.size() != 3 || isz.size() != 3) throw BasisfieldException("ZeroSplineMap::ZeroSplineMap: csz and isz must have size 3");
  _csz = csz;
  _map = new char[_csz[0]*_csz[1]*_csz[2]];
  memset(_map,0,_csz[0]*_csz[1]*_csz[2]);
  std::vector<unsigned int>  cindx(3,0);
  std::vector<unsigned int>  first(3,0);
  std::vector<unsigned int>  last(3,0);
  for (cindx[2]=0; cindx[2]<_csz[2]; cindx[2]++) {
    for (cindx[1]=0; cindx[1]<_csz[1]; cindx[1]++) {
      unsigned int bi = cindx[2]*_csz[1]*_csz[0] + cindx[1]*_csz[0];
      for (cindx[0]=0; cindx[0]<_csz[0]; cindx[0]++) {
        sp.RangeInField(cindx,isz,first,last);
        _map[bi+cindx[0]] = all_zeros(first,last,ima,isz);
      }
    }
  }
}

template<class S>
char ZeroSplineMap::all_zeros(const std::vector<unsigned int>&  first,
                              const std::vector<unsigned int>&  last,
                              const S                           *ima,
                              const std::vector<unsigned int>&  isz) const
{
  for (unsigned int k=first[2]; k<last[2]; k++) {
    for (unsigned int j=first[1]; j<last[1]; j++) {
      unsigned int bi = k*isz[1]*isz[0] + j*isz[0];
      for (unsigned int i=first[0]; i<last[0]; i++) {
        if (ima[bi+i] != static_cast<S>(0)) return(0);
      }
    }
  }
  return(1);
}

} // End namespace BASISFIELD

#endif
