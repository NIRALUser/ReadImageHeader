#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <vector>
#include <string>
#include <itkVectorImage.h>
#include <itkMetaDataObject.h>
#include "ReadImageHeaderCLP.h"

#define TypeMACRO( M )\
M ( unsigned char ) ;\
M ( char ) ;\
M ( unsigned short ) ;\
M ( short ) ;\
M ( unsigned int ) ;\
M ( int ) ;\
M ( unsigned long ) ;\
M ( long ) ;\
M ( float ) ;\
M ( double ) ;

#define MetaMACRO( T )\
{\
typedef itk::MetaDataObject< T > MetaDataType ;\
MetaDataType::Pointer entryvalue\
               = dynamic_cast< MetaDataType* >( entry.GetPointer() ) ;\
if( entryvalue )\
  {\
   T val = entryvalue->GetMetaDataObjectValue();\
   std::cout << val << std::flush ;\
  }\
}\

template< class T >
void VectorPrint( std::vector< T > vec )
{
  for( unsigned int i = 0 ; i < vec.size() ; i++ )
  {
    std::cout << vec[ i ] << " " << std::flush ;
  }
}

#define MetaVectorMACRO( T )\
{\
typedef itk::MetaDataObject< std::vector< T > > MetaDataType ;\
MetaDataType::Pointer entryvalue\
               = dynamic_cast< MetaDataType* >( entry.GetPointer() ) ;\
if( entryvalue )\
{\
   std::vector< T > val = entryvalue->GetMetaDataObjectValue();\
   VectorPrint< T >( val ) ;\
}\
}\

template< class T >
void MatrixPrint( std::vector< std::vector< T > > vec )
{
 for( unsigned int i = 0 ; i < vec.size() ; i++ )
 {
   std::cout << "(" << std::flush ;
   for( unsigned int j = 0 ; j < vec[ i ].size() ; j++ )
   {
      std::cout << vec[ i ][ j ] ;
      if( j != vec[ i ].size() - 1 )
      {
         std::cout<< "," ;
      }
   }
   std::cout << ") " << std::flush ;
 }
}

#define MetaMatrixMACRO( T )\
{\
typedef itk::MetaDataObject< std::vector< std::vector< T > > > MetaDataType ;\
MetaDataType::Pointer entryvalue\
               = dynamic_cast< MetaDataType* >( entry.GetPointer() ) ;\
if( entryvalue )\
{\
   std::vector< std::vector< T > > val = entryvalue->GetMetaDataObjectValue() ;\
   MatrixPrint<T>( val ) ;\
}\
}\

template< class T >
void Print( std::vector< T > vec , std::string name )
{
   std::cout << name << ": " << std::flush ;
   VectorPrint< T >( vec ) ;
   std::cout << std::endl ;
}

template<>
void Print( std::vector< std::vector< double > > vec , std::string name )
{
   std::cout << name << ": " << std::flush ;
   MatrixPrint< double >( vec ) ;
   std::cout << std::endl ;
}


struct InvalidChar
{
    bool operator()(char c) const {
        return !isprint((unsigned)c);
    }
};


void TrimString( std::string &val )
{
  if( !val.empty() )
  {
    for( std::size_t i = val.size() - 1 ; i > 0 ; i-- )
    {
      if( val[ i ] == '\n' || val[ i ] == '\r' )
      {
        val[ i ] = '\0' ;
      }
    }
  }
  val.erase( std::remove_if(val.begin(),val.end(),InvalidChar()), val.end() ) ;
}

//What pixeltype is the image 
void GetImageType( const char* fileName ,
                   bool verbose ,
                   itk::ImageIOBase::IOPixelType &pixelType ,
                   itk::ImageIOBase::IOComponentType &componentType ,
                   unsigned int &dim
                 )
{
   typedef itk::Image< unsigned char , 3 > ImageType ;
   itk::ImageFileReader< ImageType >::Pointer imageReader =
         itk::ImageFileReader< ImageType >::New();
   imageReader->SetFileName( fileName ) ;
   imageReader->UpdateOutputInformation() ;
   pixelType = imageReader->GetImageIO()->GetPixelType() ;
   componentType = imageReader->GetImageIO()->GetComponentType() ;
   itk::ImageIOBase::ByteOrder byte = imageReader->GetImageIO()->GetByteOrder() ;
   dim = imageReader->GetImageIO()->GetNumberOfDimensions() ;
   std::cout << "Pixel Type: " << imageReader->GetImageIO()->GetPixelTypeAsString( pixelType ) << std::endl ;
   std::cout << "Component Type: " << imageReader->GetImageIO()->GetComponentTypeAsString( componentType ) << std::endl ;
   std::cout << "Compression: " << imageReader->GetImageIO()->GetUseCompression() << std::endl ;
   std::vector< unsigned int > size ;
   std::vector< double > origin ;
   std::vector< double > spacing ;
   std::vector< std::vector< double > > direction ;
   for( unsigned int i = 0 ; i < dim ; i++ )
   {
      size.push_back( imageReader->GetImageIO()->GetDimensions( i ) ) ;
      origin.push_back( imageReader->GetImageIO()->GetOrigin( i ) ) ;
      spacing.push_back( imageReader->GetImageIO()->GetSpacing( i ) ) ;
      direction.push_back( imageReader->GetImageIO()->GetDirection( i ) ) ;
   }
   Print< unsigned int >( size , "Size" ) ;
   Print< double >( origin , "Origin" ) ;
   Print< double >( spacing , "Spacing" ) ;
   Print< std::vector< double > >( direction , "Direction" ) ;
   std::cout << "Number of Components: " << imageReader->GetImageIO()->GetNumberOfComponents() << std::endl ;
   std::cout << "Component size: " << imageReader->GetImageIO()->GetComponentSize() << std::endl ;
   if( verbose == false )
   {
     return ;
   }
   std::cout << "Byte Order: " << imageReader->GetImageIO()->GetByteOrderAsString( byte ) << std::endl ;
   itk::ImageIOBase::FileType ft = imageReader->GetImageIO()->GetFileType() ;
   std::cout << "File Type: " << imageReader->GetImageIO()->GetFileTypeAsString( ft ) << std::endl ;
   std::cout << "Image size in bytes: " << imageReader->GetImageIO()->GetImageSizeInBytes() << std::endl ;
   std::cout << "Image size in components: " << imageReader->GetImageIO()->GetImageSizeInComponents() << std::endl ;
   std::cout << "Image size in pixels: " << imageReader->GetImageIO()->GetImageSizeInPixels() << std::endl ;
   itk::MetaDataDictionary dic = imageReader->GetMetaDataDictionary() ;
   itk::MetaDataDictionary::ConstIterator itr = dic.Begin() ;
   itk::MetaDataDictionary::ConstIterator end = dic.End() ;
   while( itr != end )
   {
      std::cout << itr->first << ": " ;
      itk::MetaDataObjectBase::Pointer entry = itr->second ;
      typedef itk::MetaDataObject< std::string > MetaDataStringType ;
      MetaDataStringType::Pointer entryvalue
            = dynamic_cast< MetaDataStringType* >( entry.GetPointer() ) ;
      if( entryvalue )
      {
         std::string value = entryvalue->GetMetaDataObjectValue() ;
         TrimString( value ) ;
         std::cout << value ;
      }
      else
      {
         TypeMACRO( MetaMACRO ) ;
         TypeMACRO( MetaVectorMACRO ) ;
         typedef itk::MetaDataObject< std::vector< std::string > > MetaDataType ;
         MetaDataType::Pointer entryvalue
                     = dynamic_cast< MetaDataType* >( entry.GetPointer() ) ;
         if( entryvalue )
         {
            std::vector< std::string > vec = entryvalue->GetMetaDataObjectValue();
            for( unsigned int i = 0 ; i < vec.size() ; i++ )
            {
              std::string value = vec[ i ] ;
              TrimString( value ) ;
              std::cout << value << " " << std::flush ;
            }
          std::cout << std::endl ;
          }
          else
          {
             TypeMACRO( MetaMatrixMACRO ) ;
          }
      }
      std::cout << std::endl ;
      ++itr ;
   }
}

template< class T >
int Convert( std::string input , std::string output )
{
   typedef itk::VectorImage< T > ImageType ;
   typename itk::ImageFileReader< ImageType >::Pointer imageReader =
         itk::ImageFileReader< ImageType >::New() ;
   imageReader->SetFileName( input ) ;
   imageReader->Update() ;
   typename itk::ImageFileWriter< ImageType >::Pointer imageWriter =
         itk::ImageFileWriter< ImageType >::New() ;
   imageWriter->SetFileName( output.c_str() ) ;
   imageWriter->SetInput( imageReader->GetOutput() ) ;
   imageWriter->Update() ;
   return EXIT_SUCCESS ;
}


int main( int argc , char* argv[] )
{
   PARSE_ARGS ;
   itk::ImageIOBase::IOPixelType pixelType ;
   itk::ImageIOBase::IOComponentType componentType ;
   unsigned dim ;
   GetImageType( inputFile.c_str() , verbose , pixelType , componentType , dim ) ;
   if( outputFile.empty() )
   {
      return EXIT_SUCCESS ;
   }
   switch( componentType )
   {
      case itk::ImageIOBase::UCHAR:
         return Convert< unsigned char >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::CHAR:
         return Convert< char >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::USHORT:
         return Convert< unsigned short >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::SHORT:
         return Convert< short >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::UINT:
         return Convert< unsigned int >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::INT:
         return Convert< int >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::ULONG:
         return Convert< unsigned long >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::LONG:
         return Convert< long >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::FLOAT:
         return Convert< float >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::DOUBLE:
         return Convert< double >( inputFile.c_str() , outputFile.c_str() ) ;
         break ;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
         std::cerr << "unknown component type" << std::endl ;
         break ;
   }
   return EXIT_SUCCESS ;
}

