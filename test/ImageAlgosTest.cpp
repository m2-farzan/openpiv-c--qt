
// catch
#include "catch.hpp"

// std
#include <thread>

// local
#include "TestUtils.h"

// to be tested
#include "ImageAlgos.h"
#include "ImageLoader.h"
#include "ImageUtils.h"

using namespace Catch;

TEST_CASE("ImageAlgosTest - FFTTest")
{
    // generate sinusoidal pattern
    GFImage im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return 128 * std::sin( 2*M_PI*w/8 + 2*M_PI*h/8 ); } );

    // save
    REQUIRE( saveToFile( "fft-input.pgm", im ) );

    // transform
    FFT fft( im.size() );
    CFImage output{ fft.transform( im, Direction::FORWARD ) };

    REQUIRE( saveToFile( "fft-output.pgm", GFImage( output ) ) );

    // two peaks; relies on quadrants being unswapped i.e. zero
    // frequency is at edges
    for ( uint32_t y=0; y<output.height(); ++y )
        for ( uint32_t x=0; x<output.width(); ++x )
            if ( x == 32 && y == 32 )
            {
                auto p = Point2<uint32_t>{ x, y };
                std::cout << p << ": " << output[ p ] << "\n";
                REQUIRE( (output[ p ] != CF{}) );
            }
            else if ( x == 224 && y == 224 )
            {
                auto p = Point2<uint32_t>{ x, y };
                std::cout << p << ": " << output[ p ] << "\n";
                REQUIRE( (output[ p ] != CF{}) );
            }
            else
            {
                auto p = Point2<uint32_t>{ x, y };
                std::cout << p << ": " << output[ p ] << "\n";
                REQUIRE( (output[ p ].abs_sqr()) == Approx(0).margin(1e-9) );
            }

    // inverse transform
    GFImage g{ real( fft.transform( output, Direction::REVERSE ) ) };

    REQUIRE( saveToFile( "fft-reverse.pgm", g) );
}


TEST_CASE("ImageAlgosTest - FFT Different Size")
{
    GFImage im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return (w/2)%2 ? 1.0 : 0.0; } );

    FFT fft( { 512, 512 } );
    _REQUIRE_THROWS_MATCHES( fft.transform( im ), std::runtime_error, Contains( "image size is different" ) );
}

TEST_CASE("ImageAlgosTest - FFT non-power-of-two size")
{
    _REQUIRE_THROWS_MATCHES( FFT( { 512, 400 } ), std::runtime_error, Contains( "power of 2" ) );
    _REQUIRE_THROWS_MATCHES( FFT( { 400, 512 } ), std::runtime_error, Contains( "power of 2" ) );
}

TEST_CASE("ImageAlgosTest - FFT wrong thread")
{
    GFImage im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return (w/2)%2 ? 1.0 : 0.0; } );

    FFT fft( im.size() );
    std::thread t{ [&im, &fft](){
            _REQUIRE_THROWS_MATCHES(
                fft.transform( im ),
                std::runtime_error,
                Contains( "wrong thread" ) );
        } };
    t.join();
}

TEST_CASE("ImageAlgosTest - CrossCorrelationTest")
{
    // load images
    auto im = loadFromFile< GF >( "corr_a.tiff" );

    // extract a couple of small windows
    Size s{ 128, 128 };
    auto view_a{ createImageView( im, Rect{ {20, 20}, s } ) };
    auto view_b{ createImageView( im, Rect{ {20, 25}, s } ) };

    REQUIRE( saveToFile( "cross-correlate-a-input.pgm", view_a ) );
    REQUIRE( saveToFile( "cross-correlate-b-input.pgm", view_b ) );

    // prepare & correlate
    FFT fft( view_a.size() );
    GFImage output{ fft.cross_correlate( view_a, view_b ) };

    // write output
    REQUIRE( saveToFile( "cross-correlate-output.pgm", output ) );
}

TEST_CASE("ImageAlgosTest - AutoCorrelationTest")
{
    // load images
    auto im = loadFromFile< GF >( "corr_a.tiff" );

    // extract a couple of small windows
    Size s{ 128, 128 };
    auto view_a{ createImageView( im, Rect{ {20, 20}, s } ) };
    auto view_b{ createImageView( im, Rect{ {20, 25}, s } ) };

    // combine
    GFImage data{ view_a.width(), view_a.height() };
    data = view_a + view_b;
    REQUIRE( saveToFile( "auto-correlate-input.pgm", data) );

    // prepare & corrrelate
    FFT fft( data.size() );
    GFImage output{ fft.auto_correlate( data ) };
    REQUIRE( saveToFile( "auto-correlate-output.pgm", output) );
}

TEST_CASE("ImageAlgosTest - FFTRealTest")
{
    // load images
    auto im_a = loadFromFile< GF >( "corr_a.tiff" );
    std::cout << "im_a: " << im_a << "\n";

    // extract a couple of small windows
    Size s{ 128, 128 };
    auto view_a{ createImageView( im_a, Rect{ {20, 20}, s } ) };
    auto view_b{ createImageView( im_a, Rect{ {20, 25}, s } ) };

    // combine
    GFImage data{ s };
    data = view_a + view_b;

    REQUIRE( saveToFile( "fft_corr_a_input.pgm", data ) );

    // transform
    FFT fft( data.size() );
    CFImage output;
    output = fft.transform( data, Direction::FORWARD );

    // remove DC
    output[ {0, 0} ] = CF{};

    // transformed
    swap_quadrants( output );
    REQUIRE( saveToFile( "fft_corr_a_fourier.pgm", GFImage{ output }) );

    // complex conjugate
    for ( uint32_t h=0; h<output.height(); ++h )
        for ( uint32_t w=0; w<output.width(); ++w )
            output[ {w, h} ] = output[ {w, h} ] * output[ {w, h} ].conj();

    REQUIRE( saveToFile( "fft_corr_a_conj.pgm", GFImage{ output }) );

    // inverse
    output = fft.transform( GFImage( output ), Direction::REVERSE );
    swap_quadrants( output );

    // write output
    REQUIRE( saveToFile( "fft_corr_a_output.pgm", GFImage{ output }) );
}
