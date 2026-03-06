### build && run
cd client      # or cd srever
mkdir build
cd build
cmake -G "Ninja" ..
ninja -j8
./bin/client   # or ./bin/server
