# JUCE Fetch from GitHub 
include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/master
)
FetchContent_MakeAvailable(JUCE)
