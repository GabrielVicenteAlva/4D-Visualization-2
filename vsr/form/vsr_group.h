/*
 * =====================================================================================
 *
 *       Filename: vsr_group.h
 *
 *        Version:  1.0
 *        Created:  04/30/2015 15:30:13
 *       Revision:  none
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

/*!

@file

Point and Space Groups in 2D and 3D

Groups are defined generically on reflection pin type T, so
the techniques could theoretically be extended to build hyperbolic groups
that reflect over circles and spheres.

Examples that use this:

scratch/projects/groups/xSpaceGroup2D.cpp
scratch/projects/groups/xSpaceGroup3D.cpp

@sa vsr_root.h for generating reflection groups

@todo deprecate overloaded () operator (in favor of `apply`)
@todo fix up SpaceGroup2D so that it is as complete as SpaceGroup3D
@todo revisit application algorithm (e.g. reserve space ahead of time)
@todo only supports primitive lattice (not body centered, etc) right now
@todo bravais system type should be determinable to avoid crashing
*/

#ifndef  vsr_group_INC
#define  vsr_group_INC

#include <vsr/form/vsr_set.h>
#include <vsr/form/vsr_root.h>
#include <vsr/detail/vsr_generic_op.h>
#include <vector>

using std::vector;

namespace vsr{

/// Simple Reflection group templated on Pin Type (no translating or gliding spinors)
/// @todo make class with private member mOps
/// @todo is this used at all?
template<class V>
struct SimpleGroup{
   vector<V> ops;                       ///< Pin Operators (Vec, etc)

   SimpleGroup() {}
   SimpleGroup( vector<V> v ) : ops(v) {} ///< Instantiate with pin group

    /// Applies all std::vector<V> operators onto p motif and returns results
    /// as std::vector<T>
  template<class T>
    vector<T> operator()(const T& p){
        vector<T> res;
        for (auto& i : ops ){
           T tp = p.reflect( i ); //assuming all i's are unit
           res.push_back( tp );
           //reflect back over first mirror plane to get original ...
           res.push_back( tp.reflect( ops[0] ) );
        }
        return res;
    }
};


/// A Group of Operations called with group( sometype t ) or group( vector<sometype> t)
/// V are  versors any dimension, etc DualLines in cga2D or DualPlanes in cga3D or Circles . . .
/// NOTE this is overloaded by the PointGroup3D, which handles its own operator ()
/// @todo use OpInst instead
template< class V >
struct Group {

  /// OpInst specifies type of operator, index into operation list, and index of input
//  struct OpInst{
//    int type, opIdx, resIdx;
//  };

//  vector<OpInst> opInst;    ///< store all transformation instructions here.
//  int mNumGen;            ///< the number of generators

  int numSimple;          ///< The number of simple roots used to generate group

  /// Trs is the translator type of whatever conformal metric we are in
  using Trs = typename V::space::translator;
  using GlideType = decltype(V()*Trs());
  using ScrewType = decltype(V()*V()*Trs());

  vector<V> ops;                              ///< Pin Operators (Vec, etc)


  vector<decltype(V()*V())> sops;             ///< Spin Operators (Rot, etc)
  vector<decltype(V()*V()*V())> tops;         ///< Triple Reflection (abbar 3d group)
  vector<decltype(V()*Trs())> gops;           ///< Glide Operators
  vector<decltype(V()*V()*Trs())> scrops;     ///< Screw Operators (Motor)

  Group() {}
  Group( vector<V> v ) : ops(v) {}

  /// Generate basic set of versors (we will replace with glides and screws later)
//  virtual void seed(const V& vec =Vec(.213,.659,1.6967).unit() ) = 0;



    /// Applies all operators on p motif and returns results
    /// Note this is overloaded in PointGroups, which use a "seed-based recording" approach
//  template<class T>
//    vector<T> operator()(const T& p){
//          vector<T> res;
//
//          for (auto& i : ops ){
//              T tp = p.reflect( i.unit() );
//              res.push_back( tp );
//              //reflect EACH back over first mirror plane to get original ...
//              res.push_back( tp.reflect( ops[0].unit() ) );
//          }
//
//          for (auto& i : sops){
//            T tp = p.spin(i);
//            res.push_back(tp);
//          }
//
//          //unsure about this (abc)..
//          for (auto& i : tops){
//            T tp = p.reflect(i);
//            res.push_back(tp);
//          }
//
//
//           //apply glides and reapply pins
//           for (auto& i : gops){
//              T tg = p.reflect(i);
//              if (ops.empty()) {
//                res.push_back(tg);
//                res.push_back( tg.reflect( gops[0] ) );
//              }
//              for (auto& j : ops){
//                T tp = tg.reflect( j.unit() );
//                res.push_back(tp);
//                res.push_back( tp.reflect( ops[0].unit() ) );
//              }
//           }
//
//          return res;
//    }
//
//    /// Applies all operations on a vector of type T
//    template<class T>
//    vector<T> operator()(const vector<T>& p){
//      vector<T> res;
//      for (auto& i : p) {
//        auto tRes = (*this)(i);
//        for (auto& j : tRes){
//          res.push_back(j);
//        }
//      }
//      return res;
//    }

};

/// A 2D Point Group at the origin
/// Templated on V, the reflection type of the point group (e.g. Vec or DualPlane)
template< class V >
struct PointGroup2D : Group<V> {

    typedef decltype( V() ^ V() ) Biv;

    V a, b;                 ///< Generators

    int mP;                 ///< Symmetry Group
    bool bPin;              ///< Pin or Spin

    PointGroup2D () {}

    PointGroup2D(int p, bool pin)
    { set (p, pin); }

    virtual void set (int p, bool pin)
    {
      mP= p;
      bPin=pin;
      a = V::x;
      b = V::x.rot( Biv::xy * -PIOVERTWO/p );
      calcOps(a.unit(), b.unit());
       //setOps ();
       //seed ();
    }

   /// For pin groups, generate a reflection root system of ops
   /// Otherwise, generate mP rotors
   /// @sa vsr_root.h
   void calcOps(const V& ta, const V& tb){

       this->ops.clear();
       this->sops.clear();
       this->gops.clear();

       if (bPin)
          this->ops = Root::System(ta,tb);
       else {
        auto rot = a*b;
        auto trot = rot;
        for (int i=0;i<mP;++i){
          this->sops.push_back(trot);
          trot = trot * rot;
        }
      }


    }

  void setOps (){
     this->ops = {a,b};
     this->sops = {a*b};
  }

// 2D seed
//  virtual void seed(const V& vec =V(.213,.659,0.0).unit()){
//
//    if (!bPin) {
//      for (int i = 0; i < mP; ++i) {
//        opInst . push_back ( {1,0,0} );
//      }
//      return;
//    }
//
//    opInst = { {0,0,0}, {0,1,0} };
//
//
//    vector<Vec> tv;
//    tv.push_back(vec);
//
//    for (auto& i: opInst){
//      tv.push_back( vec.reflect( this->ops[i.opIdx]) );
//    }
//
//    int numBasicOps = 2; //opInst.size();
//
//    //REFLECTIONS RECORD
//    //Keep transforming basic seed until none are new, record results in reflectIdx
//    bool keepgoing=true; int iter=0; int maxiter = 100;
//    while(keepgoing && iter<maxiter){
//      iter++;
//      keepgoing=false;
//      int tn = tv.size();
//
//      //Any New Reflection Results
//      for(int i =0; i<numBasicOps; ++i){
//        for (int j=0;j<tn;++j){
//          tmp =  tv[j].reflect( this->ops[ opInst[i].opIdx ] );
//
//          bool exists = false;
//          for (auto& k : tv){
//            exists = Root::Compare(tmp,k);
//            if (exists) {
//              break;
//            }
//           }
//
//          //if new, add result to tv and idx
//          if (!exists){
//            tv.push_back(tmp);
//            opInst.push_back( {0, opInst[i].opIdx, j} );
//            keepgoing=true;
//          }
//        }
//      }
//    }
// }

//    // apply transformation group to a multivector T using seed method (in progress...)
//    template<class T>
//    vector<T> apply2(const T& p) const{
//
//      vector<T> res;
//      res.push_back(p);
//
//      for (auto& i : opInst){
//        T tmp;
//        T& input = res[i.resIdx];
//        switch(i.type){
//         case 0: //reflect
//            tmp = input.reflect( this->ops[ i.opIdx] );
//            break;
//         case 1: //spin
//            tmp = input.spin( this->sops[ i.opIdx] );
//            break;
//         case 3: //glide
//            tmp = input.reflect( this->gops[ i.opIdx] );
//            break;
//        }
//        res.push_back(tmp);
//      }
//
//      return res;
//    }

    size_t numOps () const {
      int o = this->ops . size ();
      int s = this->sops . size ();
      int g = this->gops . size ();

      int gg = (o == 0) ? g * 2 : o * g * 2;

      return o*2 + s + gg;
    }

    /*-----------------------------------------------------------------------------
     *  Apply translations to a single element
     *-----------------------------------------------------------------------------*/
    template<class T>
    vector<T> apply (const T& motif) const {

      vector<T> res;
      res . reserve (numOps());

      for (auto& i : this->ops ){
        //reflect motif over each pin group member
        T tm = motif.reflect( i.unit() );
        res.push_back( tm );
        //reflect EACH back over first mirror plane to get original ...
        res.push_back( tm.reflect( this->ops[0].unit() ) );
      }

      for (auto& i : this->sops ){
        T tm = motif.spin(i.unit());
        res.push_back( tm );
      }

      for (auto& i : this->gops){
        T tg = motif.reflect(i);
        if (this->ops.empty()) {
         res.push_back(tg);
         res.push_back(tg.reflect( this->gops[0] ) );
        }
        for (auto& j : this->ops){
          T tp = tg.reflect( j.unit() );
          res.push_back(tp);
          res.push_back( tp.reflect( this->ops[0].unit() ) );
        }
      }

      return res;
    }

    /// Apply to a vector of elements and striate results
    template<class T>
    vector<T> apply(const vector<T>& motif){
      vector<T> res (numOps() * motif.size ());
      int ii=0;
      for (auto& i : motif){
        auto tmp = apply(i);
        int jj=0;
        for (auto& j : tmp){
          int idx = jj*motif.size() + ii;
          res[idx]=j;
          jj++;
        }
        ii++;
      }
      return res;
    }

//      for (auto& i : motif){
//        vector<T> tRes = apply(i);
//        for (auto& j : tRes) res.push_back(j);
//      }
//      return res;
//    }

 //   template<class T>
 //   vector<T> apply(const vector<T>& p) const{
 //     vector<T> res( (opIdx.size()+1) * p.size() );
 //     int ii=0;
 //     for (auto& i : p){
 //       auto tmp = apply(i);
 //       int jj=0;
 //       for (auto& j : tmp){
 //         int idx = jj*p.size() + ii;
 //         res[idx]=j;
 //         jj++;
 //       }
 //       ii++;
 //     }
 //     return res;
 //   }


};

///  A PointGroup2D along with a lattice formed by translating along the generators a and b
///  we generate the point group calcOps() and then replace by glide reflections if called for
///  To use Translators multiplicatively, here we assume the conformal model is in play (2D or above)
///  typename V is a cga::Vec pin group
template<class V>
struct SpaceGroup2D : PointGroup2D<V> {

  using Trs = typename V::space::translator;
  using Vec = typename V::space::vector;

  typedef decltype( V() ^ V() ) Biv;

  int mDiv;  //(p)rimitive lattice = 1.0 (default); (c)entered lattice = 2; (h)exagonal lattice = 3
  float mRatioY=1;
  float mRatioX=1;

  SpaceGroup2D (){}
  /// Constructor
  // @param mP: symmetry group (1,2,3,4,6)
  // @param mRatio: for translations on the lattice
  // @param bPin: pin or spin generators
  // @param mDiv: number of divisions into lattice cell
  // @param ga: whether a glide reflection replaces first mirror generator
  // @param gb: whether a glide reflection replaces second mirror generator
  SpaceGroup2D(int p, float ratio = 1.0, bool pin=true, int div = 1, bool ga=false, bool gb=false) : PointGroup2D<V>(p, pin)
  { set (p,ratio,pin,div,ga,gb); }

  virtual void set (int p, float ratio, bool pin, int div, bool ga, bool gb){

   PointGroup2D<V>::set(p,pin);
   mRatioY = ratio; mDiv = div;

   if (p==4)
    this->b = this->a + (this->a).rot( Biv::xy * -PIOVERTWO/2);
   if (p==6)
    this->b = this->a + this->a.rot( Biv::xy * -PIOVERTWO/3);

   //Glide Reflections
   if (pin) {
     if (p==1){
       if (ga) { //replace first mirror
         this->ops.clear();
         this->gops.push_back( this->a * nga::Gen::trs( Vec::y * .5) );
       }
     } else if (p<4){
       if (ga) { //replace first mirror
         this->ops.clear();
         this->ops.push_back( this->b );
         this->gops.push_back( this->a * nga::Gen::trs(this->b * .5) );
       }
       if (gb) { //replace second mirror
         if (!this->ops.empty())
           this->ops.pop_back();
         this->gops.push_back( this->b * nga::Gen::trs(this->a * .5) );
       }
     } else {
       if (p==4){
         if(ga){
           this->ops.clear();
           this->ops.push_back(this->b.unit());
           this->ops.push_back(this->b.reflect(this->a).unit());
           this->gops.push_back( this->a * nga::Gen::trs((this->b - this->a) * .5) );
         }
       }
     }
   }
  }
    /// Get translation vector at x,y
    Vec vec(float x, float y){
      if (this->mP!=1) return this->a*x*mRatioX + this->b*y*mRatioY;
      else return this->a*x + Vec::y*y;
    }


    /*-----------------------------------------------------------------------------
     *  Apply translations to a single element
     *-----------------------------------------------------------------------------*/
    /// Hang motif on lattice points
    template<class T>
    vector<T> hang(const T& motif, int x, int y){
      vector<T> res;
      res.reserve (x*y*mDiv);
      Vec bottom_left = vec(-(x-1)/2.0, -(y-1)/2.0);

        for (int j=0; j<x; ++j){
          for (int k=0; k<y; ++k){
            for (int m =0;m<mDiv;++m){
              float t = (float)m/mDiv;
              res.push_back( motif.trs(bottom_left + vec(j,k) + vec(t,t) ) );
            }
          }
        }
      return res;
    }

    /// Hang std::vector on lattice points
    template<class T>
    vector<T> hang_old(const vector<T>& motif, int x, int y){
      vector<T> res;
      res.reserve (motif.size()*x*y*mDiv);
      Vec bottom_left = vec(-(x-1)/2.0, -(y-1)/2.0);
      for (auto& i : motif){
        auto tres = hang(i,x,y);
        res . insert (res.end (), tres.begin (), tres.end());
      }
      return res;
    }

    /// Hang std::vector on lattice points
    template<class T>
    vector<T> hang(const vector<T>& motif, int x, int y){
      vector<T> res(motif.size()*x*y*mDiv);
      Vec bottom_left = vec(-(x-1)/2.0, -(y-1)/2.0);
      int ii = 0;
      for (auto& i : motif){
        auto tmp = hang(i,x,y);
        int jj = 0;
        for (auto &j : tmp){
          int idx = jj * motif.size() +ii;
          res[idx] = j;
          jj++;
        }
        ii++;
      }
      return res;
    }

    /// Don't apply operations, just hang on lattice points before/after possibly applying bilinear function F
    template<class T, class F>
    vector<T> hang(const T& motif, int x, int y, int bPre, const F& bifunc){
      vector<T> res;
      Vec bottom_left = vec(-(x-1)/2.0, -(y-1)/2.0);

        for (int j=0; j<x; ++j){
          for (int k=0; k<y; ++k){
            for (int m =0;m<mDiv;++m){
              float t = (float)m/mDiv;
              if (bPre ==0){        //don't apply biFunc
                res.push_back(motif.trs(bottom_left + vec(j,k) + vec(t,t)));
              } else if (bPre==1) { //apply biFunc before translating
                T tres = bifunc((float)j/x, (float)k/y, motif);
                res.push_back( tres.trs(bottom_left + vec(j,k) + vec(t,t)));
              } else if (bPre==2) { //apply biFunc after translating
                T tres = motif.trs(bottom_left + vec(j,k) + vec(t,t));
                res.push_back(bifunc ((float)j/x, (float)k/y, tres));
              }
            }
          }
        }
      return res;
    }

    /// Don't apply operations, just hang on lattice points and apply bilinear function F
    template<class T, class F>
    vector<T> hang(const vector<T>& motif, int x, int y, int bPre, const F& bifunc){
      vector<T> res;
      Vec bottom_left = vec(-(x-1)/2.0, -(y-1)/2.0);
      for (auto& i : motif){
        auto tres = hang(i,x,y,bPre,bifunc);
        res . insert (res.end (), tres.begin (), tres.end());
      }
      return res;
    }

};

/// A 3D Point Group at the Origin, templated on V, the reflection pin operator type (e.g. cga::Vec)
template<class V>
struct PointGroup3D : Group<V> {

    /// specifies type of operator, index into operation list, and index of input
    struct OpIdx{
      int type, opIdx, resIdx;
    };

    /// Print out operations
    stringstream opStream(const OpIdx& i){
       stringstream s;

       if (i.type==0){
        if (i.opIdx==0) s << "a";
        else if (i.opIdx==1) s << "b";
        else if (i.opIdx==2) s << "c";
       } else if (i.type==1){
        if (i.opIdx==0) s << "ab";
        else if (i.opIdx==1) s << "bc";
        else if (i.opIdx==2) s << "ac";
       } else if (i.type==2){
        if (i.opIdx==0) s << "abc";
        else if (i.opIdx==1) s << "bac";
       } else if ( i.type==3 ){
        if (i.opIdx==0) s << "aT";
        else if (i.opIdx==1) s << "bT";
        else if (i.opIdx==2) s << "cT";
       } else if (i.type==4){
        if (i.opIdx==0) s << "abT";
        else if (i.opIdx==1) s << "bcT";
        else if (i.opIdx==2) s << "acT";
       }

       if (i.resIdx!=0) s << opStream(opIdx[i.resIdx-1]).str();

       return s;
    }

    using Biv = typename V::space::Biv;
    using Vec = typename V::space::Vec;
    using GlideType = typename Group<V>::GlideType;
    using ScrewType = typename Group<V>::ScrewType;

    V a, b, c;            ///< unit length root versors

    int mNumGen;          ///< the number of generators

    vector<OpIdx> opIdx;  ///< given a seed vector, store all transformations here.


    /// Symmetry point group data
    struct Sym{
      int p,q;
    } mSym;

    /// Bar data
    struct Bar{
      bool a, b, ab;
    } mBar;

    /// Returns true if we are dealing with the special case of p=3 and q=3
    bool is33(){
      return (mSym.p==mSym.q) && (mSym.p==3);
    }

    PointGroup3D(){};

    /// must satisfy dicycle ab^p = bc^q = ac^2
    PointGroup3D(int p, int q, bool abar=false, bool bbar=false, bool abbar=false)
    : mSym{p,q}
    {
      set(p,q,abar,bbar,abbar);
    }

    void set(int p, int q, bool abar=false, bool bbar=false, bool abbar=false){

      mSym = {p,q};
      mBar = {abar, bbar, abbar};
      /// Clear all data
      opIdx.clear();

      //0. a and c are at 90 degrees, must find b...
      a = V::x;
      c = V::y;

      //1. employ the good old spherical trig cosine rule ...
      // @todo *there is a simpler way, below, but needs to  be worked out for is33()

     //1. employ reduced version of good old spherical trig cosine rule ...
//     double tp = PI/p;
//     double tq = PI/q;
//
//     double ca = cos(tq);
//     double sa = sin(tq);
//     double cc = cos(tp);
//     double sc = sin(tp);
//
//     //reduced (because tb is contrained to PIOVERTWO)
//     double tA = acos( ca/sc );
//     double tC = acos( cc/sa );
//
//     //2. ... to rotate the yx plane ...
//     auto bivA = (a ^ c).rot( a.unduale() * -tA / 2.0 );//changed
//     auto bivC = (a ^ c).rot( c.unduale() * tC / 2.0 );
//
//
//     b = (bivA.duale() ^ bivC.duale()).duale().unit() * ( is33() ? -1 : 1 ); //note neg!
//
      //old way
      double tb = PIOVERTWO;
      double ta = is33() ? -PI/(int)p : PI/(int)p;
      double tc = is33() ? PI/(int)q : -PI/(int)q;

      double ca = cos(ta);
      double sa = sin(ta);
      double cb = cos(tb);
      double sb = sin(tb);
      double cc = cos(tc);
      double sc = sin(tc);

      double tA = acos( (ca-(cb*cc))/(sb*sc) );
      double tC = acos( (cc-(ca*cb))/(sa*sb) );

      //2. ... to rotate the yx plane ...
      auto bivA = (a ^ c).rot( a.duale() * -tC / 2.0 );
      auto bivC = (a ^ c).rot( c.duale() * -tA / 2.0 );//changed

      //3. ... and find b via coincidence of planes ...
      b = (bivA.duale() ^ bivC.duale()).unduale().unit()  * ( is33() ? -1 : 1 ); //note neg!

      setOps();


      if (!abar && !bbar && !abbar ){                    // Case of only reflections. easy!
         opIdx= {  {0,0,0}, {0,1,0}, {0,2,0} };
       } else if (abbar){                                // Case of roto-reflection
         opIdx = { {2,0,0} };
       } else if ( abar && bbar ) {
         opIdx = { {1,0,0}, {1,1,0} };
       } else if (abar){                                 // Case of only one or other are cyclic
         opIdx = { {0,2,0},{1,0,0} };
       } else if (bbar){
         opIdx = { {0,0,0},{1,1,0} };
       } //note need an ac bar option for space groups

         // seed a random vector to "record" all unique reflection and spin operations
         seed();

         mNumGen = opIdx.size();

    }


    /// Store set of operating versors
    void setOps(){
      this->ops = {a,b,c};            // OpInst type: 0
      this->sops = {a*b, b*c, a*c};   // OpInst type: 1
      this->tops = {a*b*c, b*a*c};    // OpInst type: 2
      this->gops = vector<GlideType>(3);
      this->scrops = vector<ScrewType>(3);
    }

    /// Generate basic set of versors (we will replace with glides and screws later)
    void seed(const Vec& vec=Vec(.213,.659,1.6967).unit() ){

        vector<Vec> tv;
        tv.push_back(vec);
        Vec tmp;

        for (auto& i: opIdx){
          switch (i.type){
            case 0:
               tmp = vec.reflect( this->ops[i.opIdx]);
               break;
            case 1:
               tmp = vec.spin( this->sops[i.opIdx]);
               break;
            case 2:
               tmp = vec.reflect( this->tops[i.opIdx] );
               break;
          }
          tv.push_back(tmp);
        }


        //tops? gops?
        int numBasicOps = opIdx.size();
        //REFLECTIONS RECORD
        //Keep doing it until none new, record results in reflectIdx
        bool keepgoing=true; int iter=0; int maxiter = 100;
        while(keepgoing && iter<maxiter){
          iter++;
          keepgoing=false;
          int tn = tv.size();

          //Any New Reflection Results
          for(int i =0; i<numBasicOps; ++i){
            for (int j=0;j<tn;++j){

              switch (opIdx[i].type){
                  case 0:
                    tmp =  tv[j].reflect( this->ops[ opIdx[i].opIdx ] );
                    break;
                  case 1:
                    tmp = tv[j].spin( this->sops[opIdx[i].opIdx] );
                    break;
                  case 2:
                    tmp = tv[i].reflect( this->tops[opIdx[i].opIdx] );
                    break;
              }

              bool exists = false;
              for (auto& k : tv){
                exists = Root::Compare(tmp,k);
                if (exists) {
                  break;
                }
               }

              //if new, add result to tv and idx
              if (!exists){
                tv.push_back(tmp);
                opIdx.push_back( {opIdx[i].type, opIdx[i].opIdx, j} );
                keepgoing=true;
              }
            }
          }
        }
    }

    /// apply to a std::vector and striate results
    template<class T>
    vector<T> apply(const vector<T>& p) const{
      vector<T> res( (opIdx.size()+1) * p.size() );
      int ii=0;
      for (auto& i : p){
        auto tmp = apply(i);
        int jj=0;
        for (auto& j : tmp){
          int idx = jj*p.size() + ii;
          res[idx]=j;
          jj++;
        }
        ii++;
      }
      return res;
    }

    // apply transformation group to a multivector T
    template<class T>
    vector<T> apply(const T& p) const{

      vector<T> res;
      res.push_back(p);

      for (auto& i : opIdx){
        T tmp;
        T& input = res[i.resIdx];
        switch(i.type){
         case 0: //reflect
              tmp = input.reflect( this->ops[ i.opIdx] );
              break;
         case 1: //spin
              tmp = input.spin( this->sops[ i.opIdx] );
              break;
          case 2: //roto-reflect (invert)
              tmp = input.reflect( this->tops[i.opIdx] );
              break;
          case 3: //glide
              tmp = input.reflect( this->gops[ i.opIdx] );
              break;
          case 4: //screw
              tmp = input.spin( this->scrops[ i.opIdx] );
              break;

        }
        res.push_back(tmp);
      }

      return res;
    }



    auto ab() RETURNS ( a * b )
    auto ac() RETURNS ( a * c )
    auto bc() RETURNS ( b * c )

};


/// 3D space group, templated on mirror type (e.g. cga::Vec )
// avoid 6.3 5.3
template<class V>
struct SpaceGroup3D : PointGroup3D<V> {

    using Trs = typename V::space::translator;
    using Vec = typename V::space::Vec;
    using Biv = typename V::space::Biv;

    /// Lattice cell system
    enum BravaisType{
      Triclinic=1, Monoclinic, Orthorhombic, Tetragonal, Trigonal, Hexagonal, Cubic
    };

    /// Lattice cell type
    enum LatticeType{
      Primitive=1, Body, SingleFace, Face, Rhombo
    };

    /// Cell type and system
    struct Lattice{
      int system; //bravais system (Triclinic, Tetragonal, etc)
      int type;   //latticetype (primitive, Body, single face, etc)
    };

    /// A axial (a), B axial (b), C axial (c), Diagonal (n), Diamond (d)
    enum GlideType{
      None=0, AxialA, AxialB, AxialC, Diagonal, Diamond
    };
    /// Glide type and boolean for subtype
    struct GlideParameter{
      int type; //
      bool bInvert;   //boolean switch for b or a-b, b+c or a-b+c
    };
    /// Glide types (axial a b c, diagonal or diamond)
    struct Glide{
      GlideParameter a, b, c;
    };
    /// Screw data
    struct Screw{
      int ab,bc,ac;
      int ab_trs, bc_trs, ca_trs;
    };


   // Sym mSym;           ///< Symmetry Class (i.e. 4,3), from which lattice system can be deduced
    Lattice mLattice;   ///< system (Triclinic, Cubic etc) and lattice type (Primitive, Body, etc)
    Glide mGlide;       ///< glide reflections
    Screw mScrew;       ///< screw displacement

    Vec mA_length, mB_length, mC_length;       //actual (non-unit)
    Vec mA_dir, mB_dir, mC_dir;                //actual cell edge
    double mB_angle, mC_angle;                 //angle offsets of lattice for triclinic and monoclinic

    void angleB(double t){ mB_angle = t; dirB(); lengthB(); }
    void angleC(double t){ mC_angle = t; dirC(); lengthC(); }

    Vec mRatio;      ///< ratios along a, b and c

   // vector<Vec> glides[(3);   ///< upto 3 glides;

    void ratio(double x, double y, double z){
      mRatio = Vec(x,y,z);
      dirB(); lengthB();
      dirC(); lengthC();
    }

    SpaceGroup3D(){}

    /**
    * @brief 3D SpaceGroup Generator
    *
    * @param p
    * @param q
    * @param abar default false
    * @param bbar default false
    * @param abbar default false
    * @param lattice default {Triclinic, Primitive}
    * @param ratio  default Vec(1,1,1)
      @param glide default 0
      @param screw defulat 0
    */
    SpaceGroup3D(
      int p, int q,
      bool abar=false, bool bbar=false, bool abbar=false,
      Lattice lattice = {Triclinic, Primitive},
      Vec ratio= Vec(1,1,1),
      Glide glide = Glide(),
      Screw screw = {0,0,0,0,0,0}
      )
      {
        set(p,q,abar,bbar,abbar,lattice,ratio,glide,screw);
      }

    void set( int p, int q, bool abar, bool bbar, bool abbar, Lattice lattice, Vec ratio, Glide glide, Screw screw){

      PointGroup3D<V>::set(p,q,abar,bbar,abbar);
      mLattice = lattice;
      mRatio = ratio;
      mGlide = glide;
      mScrew = screw;
      init();
    }

    void print(){
         if (this->is33()) printf("33\n");
         switch(mLattice.system){
            case Triclinic:
              printf("Triclinic\n"); break;
            case Monoclinic:
              printf("Monoclinic\n"); break;
            case Orthorhombic:
              printf("Orthorhombic\n"); break;
            case Tetragonal:
              printf("Tetragonal\n"); break;
            case Trigonal:
              printf("Trigonal\n"); break;
            case Hexagonal:
              printf("Hexagonal\n"); break;
            case Cubic:
              printf("Cubic\n"); break;
         }
         switch(mLattice.type){
            case Primitive:
              printf("Primitive\n"); break;
            case Body:
              printf("Body\n"); break;
            case SingleFace:
              printf("SingleFace\n"); break;
            case Face:
              printf("Face\n"); break;
            case Rhombo:
              printf("Rhombo\n"); break;
         }

         for (auto& i : this->opIdx) {
           cout << this->opStream(i).str() << endl;
         }
    }

 //   void setCrystalSystem(){
 //     switch (mSym.p){
 //       case 1:
 //       case 2:
 //         break;
 //       case 3:
 //         mLattice.system = Cubic;
 //         break;
 //       case 4:
 //         if (mSym.q==3) mLattice.system = Cubic;
 //         break;
 //
 //     }
 //   }

    /**
    * @brief Calculate glide a Vector
    *
    * @return cga3D::Vec
    */
    Vec glideA(){
      //vector<Vec> res;
      switch(mGlide.a.type){
        case AxialB:// "b" replace a reflection with a glide along b edge (NOT in clinic, trigonal, hexagonal)
          return mB_dir;
        case AxialC:
          return mC_dir;
        case Diagonal:
          return this->is33() ? (mB_dir + mC_dir - mA_dir)  : (mB_dir+mC_dir);
        case Diamond:
          return (this->is33() ? (mB_dir + mC_dir - mA_dir)  : (mB_dir+mC_dir) ) * .5;
      }

      return Vec();
    }

    /**
    * @brief Calculate glide b Vector
    *
    * @return cga3D::Vec
    */
    Vec glideB(){
      switch(mGlide.b.type){
        case AxialA:
          return mA_dir;
        case AxialC:
          return mC_dir;
        case Diagonal:
          return mLattice.system==Cubic ? mC_dir + mA_dir - mB_dir : mC_dir + mA_dir;
        case Diamond:
          return (mLattice.system==Cubic ? mC_dir + mA_dir - mB_dir : mC_dir + mA_dir) * .5;
      }

      return Vec();
    }



    /**
    * @brief Calculate glide c Vector
    *
    * @return cga3D::Vec
    */
    Vec glideC(){
      switch(mGlide.c.type){
        case AxialA:
          return mA_dir;
        case AxialB:
          return mB_dir;
        case Diagonal:
          return mLattice.system==Cubic ? ( this->is33() ? mC_dir + mA_dir + mB_dir : mC_dir + mA_dir - mB_dir ) : mB_dir + mA_dir;
        case Diamond:
          return (mLattice.system==Cubic ? ( this->is33() ? mC_dir + mA_dir + mB_dir : mC_dir + mA_dir - mB_dir ) : mB_dir + mA_dir)  * .5;
      }

      return Vec();

    }


    /**
    * Calculate Glide Reflections
    */
    void setGlides(){
        //GLIDE replacements for non-symmorphic groups
        vector<int> replaceReflection; //keep track of indices into this->ops to replace

        if (mGlide.a.type != None)  {
            replaceReflection.push_back(0);
            auto trs = nga::Gen::trs( glideA()* .5 );
            this->gops[0] = this->a * trs;
        }

        if(mGlide.b.type != None){
            replaceReflection.push_back(1);
            auto trs = nga::Gen::trs( glideB()* .5 );
            this->gops[1] = this->b * trs;
         }

        if(mGlide.c.type != None){
            replaceReflection.push_back(2);
            auto trs = nga::Gen::trs( glideC()* .5 );
            this->gops[2] = this->c * trs;
         }

        // replace reflections with glide reflections
        for (auto& i : replaceReflection){
          for (auto& j : this->opIdx){
            if (j.opIdx==i && j.type==0 ) j.type = 3;
         }
        }
    }

    //replace rotations with screws
    void setScrews(){

        vector<int> replaceRotation;

        if (mScrew.a){
          replaceRotation.push_back(0);
          auto vec = mC_dir * (float)mScrew.a/this->mSym.p;

          auto scr = this->sops[0] * nga::Gen::trs( vec );
          this->scrops[0] = scr;
        }


        // replace rotations with screw displacements
        for (auto& i : replaceRotation){
          for (auto& j : this->opIdx){
            if (j.opIdx==i && j.type == 1) j.type = 4;
         }
        }

    }

    // Q: do we need a method to seed non-symmorphic space groups in general position and eliminate redundancies?
    void init(){

        // determine system based on Sym p and q
        // setCrystalSystem();

         setCell();

         mB_angle = 0;
         mC_angle = 0;

         setGlides();

        //replace rotation with screw
        vector<int> replaceRotation;

        if (mScrew.ab){

          replaceRotation.push_back(0);

          //switcher
          float ratio = (float)mScrew.ab/this->mSym.p;
          auto vec = mC_dir * ratio;
          auto scr = this->sops[0] * nga::Gen::trs( vec );

          //possible trans of scr
          if (mScrew.ab_trs){
            if (mScrew.ab_trs==1) scr = scr.translate( -mB_dir * (1-ratio) );
            else scr = scr.translate( mA_dir * (1-ratio) );
          }

          this->scrops[0] = scr;
        }


        if (mScrew.bc){

          replaceRotation.push_back(1);

          //switcher
          float ratio = (float)mScrew.bc/this->mSym.q;
          auto vec = mA_dir * ratio;
          if (mLattice.system == Tetragonal){
            vec = (mA_dir - mB_dir) * ratio;
          }

          auto scr = this->sops[1] * nga::Gen::trs( vec );

          //possible trans of scr
          if (mScrew.bc_trs){
            if (mScrew.bc_trs==1) scr = scr.translate( mB_dir * .25 );
            else scr = (typename Group<V>::ScrewType(this->sops[1])).translate( mB_dir * .25 );  //see e.g. 198 and 199 in hestenes/holt paper
          }


          this->scrops[1] = scr;
        }

        if (mScrew.ac){

           replaceRotation.push_back(2);

           //switcher
           // auto vec = this->a * mRatio[0] * mScrew.b;
           auto vec = mB_dir * .25;
           auto scr = this->sops[2] * nga::Gen::trs( vec );
           this->scrops[2] = scr;

         // this->scrops[1] = tmp;
        }


        // replace rotations with screw displacements
        for (auto& i : replaceRotation){
          for (auto& j : this->opIdx){
            if (j.opIdx==i && j.type == 1) j.type = 4;
         }
        }


    }


    //test for cubic 33 system (special)
 //   bool is33(){
 //      return (mLattice.system==Cubic) && FERROR( fabs((this->b<=this->a)[0]) - fabs((this->b<=this->c)[0]) );
 //   }

    void setCell(){

       if (this->is33()){
         auto rota = nga::Gen::ratio( this->a, Vec(1,0,-1).unit() );
         auto rotb = nga::Gen::ratio( this->c.spin(rota), Vec(-1,0,-1).unit() );
         auto rot = rotb*rota;
         this->a = this->a.spin(rot);
         this->b = this->b.spin(rot);
         this->c = this->c.spin(rot);
       }

       this->setOps();

        dirA();
        dirB();
        dirC();

        mA_length = lengthA();
        mB_length = lengthB();
        mC_length = lengthC();

    }

    /// get edge of lattice in a direction
    Vec dirA(){
      Vec tmp = this->a * mRatio[0];
      if (this->is33())
        tmp = (this->a - this->c) * mRatio[0]/2.0;
      mA_dir = tmp;
      return mA_dir;
    }

    /// get corrected direction of b vec along bravais lattice
    /// is stored in mB_dir on init();
    Vec dirB(){
      Vec tmp;
      switch(mLattice.system){
       case Triclinic:
       case Monoclinic:
        tmp = this->b.rotate( Biv::xz * mB_angle ) * mRatio[1];
        break;
       case Orthorhombic:
       case Trigonal:
        tmp = this->b * mRatio[0];
        break;
       case Tetragonal:
       case Hexagonal: //* ratio?
        tmp = this->a.reflect( (this->b^this->c).duale().unit() ) * mRatio[0];
        break;
       case Cubic:
        if (this->is33() ) tmp=(this->c + this->a)*mRatio[0]/2.0;
        else tmp = (-this->a.reflect(this->b))*mRatio[0];
        break;
      }

      mB_dir = tmp;
      return mB_dir;
    }

    Vec dirC(){
      Vec tmp;
      switch(mLattice.system){
       case Triclinic:
        tmp = this->c.rotate( Biv::xy * mC_angle ) * mRatio[2];
        break;
       case Cubic:
        if (this->is33()) tmp=((this->b * 2) - this->a - this->c)*mRatio[0]/2.0;
        else tmp = ((this->a ^ this->b).duale().unit())*mRatio[0];
        break;

       default:
        tmp = this->c * mRatio[2];
        break;
      }

      mC_dir = tmp;
      return mC_dir;
    }

    Vec lengthA(){
      return this->a * mRatio[0];
    }

     /// corrected length of b
    /// is stored as mB_length on init();
    Vec lengthB(){
     Vec tmp;
     switch(mLattice.system){
       case Triclinic:
       case Monoclinic:
       case Orthorhombic:
       case Trigonal:
        tmp = mB_dir;
        break;
       case Tetragonal:
       case Hexagonal:
        tmp = mA_dir + mB_dir;
        break;
       case Cubic:
         if ( this->is33() ) tmp = this->b*mRatio[0];
         else
           tmp = mA_dir + mB_dir;
          break;

      }
      mB_length = tmp;
      return tmp;
    }

     /// corrected length of c
    /// is stored as mC_length on init();
    Vec lengthC(){
     Vec tmp;
     switch(mLattice.system){
       case Cubic:
        if ( this->is33() ) tmp = this->c*mRatio[0];
        else tmp = mB_dir + mC_dir;
        break;
       default:
        tmp = mC_dir;
        break;
      }
      mC_length = tmp;
      return tmp;
    }


    /*!-----------------------------------------------------------------------------
     * Calculate a vector transformation basice on generators and ratio
     *-----------------------------------------------------------------------------*/
    Vec vec(float x, float y, float z){
        return mA_dir*x + mB_dir*y + mC_dir*z;
    }


    /*!-----------------------------------------------------------------------------
     *  Hang a motif on a Lattice of dimensions x,y,z
     *-----------------------------------------------------------------------------*/
    template<class T>
    vector<T> hang(const T& motif, int x, int y, int z){

        vector<T> res;

        switch( mLattice.type ){
            case Primitive:
            {
              for (int j=-x/2.0;j<x/2.0;++j){
                for (int k=-y/2.0;k<y/2.0;++k){
                  for (int l=-z/2.0;l<z/2.0;++l){
 //             for (int j=0;j<x;++j){
 //               for (int k=0;k<y;++k){
 //                 for (int l=0;l<z;++l){
 //                   //auto vec = vec(j,k,l);
                    res.push_back( motif.trs( vec(j,k,l) ) );
                    //if (l==z-1) res.push_back( motif.trs( cellBack() ) );
                  }
                }
              }
              break;
            }
            case Body:
            {
//             for (int j=-x/2.0;j<x/2.0;++j){
//               for (int k=-y/2.0;k<y/2.0;++k){
//                 for (int l=-z/2.0;l<z/2.0;++l){
                for (int j=0;j<x;++j){
                 for (int k=0;k<y;++k){
                  for (int l=0;l<z;++l){

                  res.push_back( motif.trs( vec(j,k,l) ) );
                  res.push_back( motif.trs( vec(j,k,l) + vec(.5,.5,.5) ) );
                 }
               }
             }
             break;
           }
          case SingleFace:
          {
            switch(mLattice.system){
              case Monoclinic:
              {
 //                for (int j=-x/2.0;j<x/2.0;++j){
 //                  for (int k=-y/2.0;k<y/2.0;++k){
 //                    for (int l=-z/2.0;l<z/2.0;++l){
               for (int j=0;j<x;++j){
                for (int k=0;k<y;++k){
                  for (int l=0;l<z;++l){

                       res.push_back( motif.trs( vec(j,k,l)  ) );
                       res.push_back( motif.trs( vec(j,k,l) + vec(0,.5,.5) ) );
                     }
                   }
                 }
                break;
              }
             case Orthorhombic:
             {
 //               for (int j=-x/2.0;j<x/2.0;++j){
 //                 for (int k=-y/2.0;k<y/2.0;++k){
 //                   for (int l=-z/2.0;l<z/2.0;++l){
               for (int j=0;j<x;++j){
                for (int k=0;k<y;++k){
                  for (int l=0;l<z;++l){

                      res.push_back( motif.trs( vec(j,k,l)  ) );
                      res.push_back( motif.trs( vec(j,k,l) + vec(.5,.5,0) ) );
                    }
                  }
                }
               break;
             }
              case Trigonal:
              case Hexagonal:
              {
//                 for (int j=-x/2.0;j<x/2.0;++j){
//                   for (int k=-y/2.0;k<y/2.0;++k){
//                     for (int l=-z/2.0;l<z/2.0;++l){
              for (int j=0;j<x;++j){
                for (int k=0;k<y;++k){
                  for (int l=0;l<z;++l){

                       res.push_back( motif.trs( vec(j,k,l)  ) );
                       res.push_back( motif.trs( vec(j,k,l) + vec(.3333333,.3333333,0) ) );
                       res.push_back( motif.trs( vec(j,k,l) + vec(.6666666,.6666666,0) ) );
                     }
                   }
                 }
                break;
              }
            }
            break;
          }
          case Face:
          {
            switch(mLattice.system){
              case Orthorhombic:
              case Cubic:
               {
//                  for (int j=-x/2.0;j<x/2.0;++j){
//                    for (int k=-y/2.0;k<y/2.0;++k){
//                      for (int l=-z/2.0;l<z/2.0;++l){
               for (int j=0;j<x;++j){
                 for (int k=0;k<y;++k){
                   for (int l=0;l<z;++l){

                        res.push_back( motif.trs( vec(j,k,l)  ) );
                        for (int m =1;m<2;++m){
                         float t = (float)m/2;
                         res.push_back( motif.trs( vec(j,k,l) + vec(t,t,0) ) );
                         res.push_back( motif.trs( vec(j,k,l) + vec(0,t,t) ) );
                         res.push_back( motif.trs( vec(j,k,l) + vec(-t,0,t) ) );
                        }
                      }
                    }
                  }
                 break;
               }
              case Trigonal:
              case Hexagonal:
                {
//                   for (int j=-x/2.0;j<x/2.0;++j){
//                     for (int k=-y/2.0;k<y/2.0;++k){
//                       for (int l=-z/2.0;l<z/2.0;++l){
              for (int j=0;j<x;++j){
                for (int k=0;k<y;++k){
                  for (int l=0;l<z;++l){

                         res.push_back( motif.trs( vec(j,k,l)  ) );
                         for (int m =1;m<3;++m){
                          float t = (float)m/3;
                          res.push_back( motif.trs( vec(j,k,l) + vec(t,t,0) ) );
                          res.push_back( motif.trs( vec(j,k,l) + vec(0,t,t) ) );
                          res.push_back( motif.trs( vec(j,k,l) + vec(-t,0,t) ) );
                         }
                       }
                     }
                   }
                  break;
                }
              }
              break;
          }
          case Rhombo:
          {
//             for (int j=-x/2.0;j<x/2.0;++j){
//               for (int k=-y/2.0;k<y/2.0;++k){
//                 for (int l=-z/2.0;l<z/2.0;++l){
              for (int j=0;j<x;++j){
                for (int k=0;k<y;++k){
                  for (int l=0;l<z;++l){

                   res.push_back( motif.trs( vec(j,k,l)  ) );
                   res.push_back( motif.trs( vec(j,k,l) + vec(.3333333,.3333333,.3333333) ) );
                   res.push_back( motif.trs( vec(j,k,l) + vec(.6666666,.6666666,.6666666) ) );
                 }
               }
             }
            break;
          }
        }
      return res;
    }

    template<class T>
    vector<T> hang(const vector<T>& motif, int x, int y, int z ){
      vector<T> res( motif.size() * x * y * z );
      int ii=0;
      for (auto& i : motif){
        auto tmp = hang(i,x,y,z); // hang each point of incoming pointgroup
        int jj=0;
        for (auto& j : tmp) {
          int idx = jj*motif.size()+ii;   //find idx into results
          res[idx]=j;
          jj++;
        }
        ii++;
      }
      return res;
    }

//    /// a std::vector of Points representing cell positions of a single lattice
//    vector<cga::Point> cellPositions(){
//
//      vector<cga::Point> res;
//    //  switch( ){
//    //    case Primitive:
//
//    //     break;
//    //    case Body:
//    //      break;
//    //    case SingleFace:
//    //      break;
//    //    case Face:
//    //      break;
//    //    case Rhombic:
//    //      break;
//    //  }
//
//      return res;
//    }

};


} //vsr::
#endif   /* ----- #ifndef vsr_group_INC  ----- */


