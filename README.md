- Configure (Debug): `cmake -S . -B out/Debug -G Ninja`
- Configure (Release): `cmake -S . -B out/Release -G Ninja -DCMAKE_BUILD_TYPE=Release`

- Build: `cmake --build out/Debug`

- Execute: `./out/rayray`