# GammaScalping
This is a volatility trading strategy. Buying the straddle when implied vol is subsided, at the hope that it will spike in the near future. At the same time, we delta hedge our portfolio to remove the affect of underlying movement on portfolio.

You will need PostgreSQL C++ library libpqxx and QuantLib to compile. You will also need include Boost library header file on search path, and link libboost_filesystem.a and libboost_system.a.
