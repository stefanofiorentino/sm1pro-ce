== SM1/PRO Community Edition==

# protocol_buffer_cpp
first working example of protocol buffer in c++11

# example to build cpp definition files
protoc -I=protos --cpp_out=. protos/benchmarks.proto

protoc -I=protos --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin protos/sm1pro.proto
