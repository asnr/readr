#ifndef FASTREAD_FASTREAD_H
#define FASTREAD_FASTREAD_H

#include <Rcpp.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <tr1/unordered_map>

namespace fastread {
    
    class MMapReader ;
    
    class VectorInput {
    public:
        VectorInput( MMapReader& reader_) : reader(reader_) {}
        virtual ~VectorInput(){} ;
        
        virtual void set( int i ) = 0 ;
        
        virtual SEXP get() = 0;
        virtual bool skip() const { return false ; }
        virtual bool is_rownames() const { return false ; }
    protected:
        MMapReader& reader ;
    } ;
    
    class VectorInput_Integer : public VectorInput {
    public:
        VectorInput_Integer( int n, MMapReader& reader_ ) : 
            VectorInput(reader_), data( Rcpp::no_init(n) ) {}
        void set( int i ) ;
        inline SEXP get(){ return data ; } 
        
    private:
        Rcpp::IntegerVector data ;
    } ;
    
    class VectorInput_Factor : public VectorInput {
    public:
        VectorInput_Factor( int n, MMapReader& reader_ ) : VectorInput(reader_), 
            data( Rcpp::no_init(n) ), 
            level_map(), 
            max_level(0)
            {}
          
        void set( int i);
        SEXP get() ;
            
    private:
        Rcpp::IntegerVector data ;
        typedef std::tr1::unordered_map<SEXP, int> MAP;  
        MAP level_map ;
        int max_level ;
    } ;
    
    class VectorInput_Double : public VectorInput {
    public:
        VectorInput_Double( int n, MMapReader& reader_ ) : 
            VectorInput(reader_), data(Rcpp::no_init(n) ){}
        void set( int i ) ;
        inline SEXP get(){ return data ; } 
        
    private:
        Rcpp::DoubleVector data ;
    } ;
    
    class VectorInput_String : public VectorInput {
    public:
        VectorInput_String( int n, MMapReader& reader_ ) : 
            VectorInput(reader_), data(Rcpp::no_init(n) ){}
        void set( int i ) ;
        inline SEXP get(){ return data ; } 
        
    private:
        Rcpp::CharacterVector data ;
    } ;
    
    class VectorInput_Rownames : public VectorInput_String {
    public:
        VectorInput_Rownames( int n, MMapReader& reader_ ) : VectorInput_String(n, reader_){}
        bool is_rownames() const { return true ; }
    } ;
    
    class VectorInput_Skip : public VectorInput {
    public:
        VectorInput_Skip( int , MMapReader& reader_ ) : VectorInput(reader_){}
        void set( int i ) ;
        inline SEXP get(){ return R_NilValue ; } 
        virtual bool skip() const { return true ; }
    } ;
    
    VectorInput* make_vector_input( int rtype, int n, MMapReader& ) ;
    VectorInput* make_vector_input( const std::string& clazz, int n, MMapReader& ) ;
        
    class MMapReader {
       public:
           MMapReader( const std::string& filename, char sep_ = ',', char quote_ = '"', char esc_ = '\\' ) ;
           ~MMapReader() ;
           
           void read(int n_, Rcpp::CharacterVector classes ) ;
           void setup(int n_, Rcpp::CharacterVector classes ) ;
           
           Rcpp::List get() ;
           
           inline char* get_pointer() const { return p; }
           inline void  set_pointer(char* p_) { p = p_ ; }
           
           int move_until_next_token_start() ;
           
           int get_int() ;
           double get_double() ;
           double get_double_fast_atof() ;
           int get_int_naive() ;
           Rcpp::String get_String() ;
           
           int count_lines() const ; 
           
           Rcpp::NumericVector parseDouble_strtod(int nd) ;
           Rcpp::NumericVector parseDouble_fast_atof(int nd) ;
           Rcpp::NumericVector parseDouble_qi(int nd) ;
           Rcpp::NumericVector parseDouble_atof(int nd) ;
           
           Rcpp::IntegerVector parseInt_strtol(int nd) ;
           Rcpp::IntegerVector parseInt_naive(int nd) ;
           Rcpp::IntegerVector parseInt_qi(int nd) ;
           
       private:
           
           int file_descriptor ;
           char sep, quote, esc ;
           
           char* memory_start ;
           char* p ;
           char* end ;
           char* eof ;
           size_t filesize ;
           bool inquote ;
           
           int n, ncol ;
           std::vector<VectorInput*> inputs ;
    } ;
    
}     

#endif
