# vortex-server
An end-to-end open source platform for distributed learning simulation.

# Installation
For using vortex-server, follow the instruction: 
- Clone the repository with option `--recursive`
- We use [gRPC](https://grpc.io) for communication between server and clients. So, install gRPC from [here](https://grpc.io/docs/languages/cpp/quickstart/).
- After installation of gRPC, you may need to modify your bash file. (My shell is zsh so I modified `~/.zshrc`)

```
echo "export GRPC_INSTALL_DIR=/path/to/grpc/.local" >> ~/.zshrc
echo "export PATH=\"$GRPC_INSTALL_DIR/bin:$PATH\"" >> ~/.zshrc
 ```
- Now, you have all of the requirement for your server to be up.
```
mkdir build; cd build;
cmake ..
make -j all
```
- Now you only need to run `./server` in `build` directory.
- If you see this output, it means that your server is up.
```
Server listen on localhost:50051
```