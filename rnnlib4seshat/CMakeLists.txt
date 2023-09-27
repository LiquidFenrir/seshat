
# Source code files
# find source -type f | grep "\.cpp$" | clip
set(RNN_LIB_SRCS
    source/ClassificationLayer.cpp
    source/DataExporter.cpp
    source/Layer.cpp
    source/Mdrnn.cpp
    source/Optimiser.cpp
    source/Random.cpp
    source/WeightContainer.cpp
)
# find public -type f | grep "\.hpp$" | clip
set(RNN_LIB_INTERFACES
    public/rnnlib4seshat/ActivationFunctions.hpp
    public/rnnlib4seshat/BiasLayer.hpp
    public/rnnlib4seshat/BlockLayer.hpp
    public/rnnlib4seshat/ClassificationLayer.hpp
    public/rnnlib4seshat/CollapseLayer.hpp
    public/rnnlib4seshat/ConfigFile.hpp
    public/rnnlib4seshat/Connection.hpp
    public/rnnlib4seshat/Container.hpp
    public/rnnlib4seshat/CopyConnection.hpp
    public/rnnlib4seshat/DataExporter.hpp
    public/rnnlib4seshat/DataSequence.hpp
    public/rnnlib4seshat/FullConnection.hpp
    public/rnnlib4seshat/GatherLayer.hpp
    public/rnnlib4seshat/Helpers.hpp
    public/rnnlib4seshat/IdentityLayer.hpp
    public/rnnlib4seshat/InputLayer.hpp
    public/rnnlib4seshat/Layer.hpp
    public/rnnlib4seshat/Log.hpp
    public/rnnlib4seshat/LstmLayer.hpp
    public/rnnlib4seshat/Matrix.hpp
    public/rnnlib4seshat/Mdrnn.hpp
    public/rnnlib4seshat/MultiArray.hpp
    public/rnnlib4seshat/MultilayerNet.hpp
    public/rnnlib4seshat/Named.hpp
    public/rnnlib4seshat/NetcdfDataset.hpp
    public/rnnlib4seshat/NetworkOutput.hpp
    public/rnnlib4seshat/NeuronLayer.hpp
    public/rnnlib4seshat/Optimiser.hpp
    public/rnnlib4seshat/Random.hpp
    public/rnnlib4seshat/RealType.hpp
    public/rnnlib4seshat/Rprop.hpp
    public/rnnlib4seshat/SeqBuffer.hpp
    public/rnnlib4seshat/SoftmaxLayer.hpp
    public/rnnlib4seshat/SteepestDescent.hpp
    public/rnnlib4seshat/String.hpp
    public/rnnlib4seshat/WeightContainer.hpp
)

add_library(seshat_lib_rnnlib4seshat STATIC
    ${RNN_LIB_SRCS}
    ${RNN_LIB_INTERFACES}
)
add_library(seshat::rnnlib4seshat ALIAS seshat_lib_rnnlib4seshat)

seshat_add_library_target_options(seshat_lib_rnnlib4seshat)

target_include_directories(seshat_lib_rnnlib4seshat
    PUBLIC
        public
)
