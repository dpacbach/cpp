CXXFLAGS += -std=c++1z

LDFLAGS += -lstdc++fs

main.deps      = xml-utils
xml-utils.deps = pugixml util

main_is = main

$(call enter_all,util xml-utils)
$(call make_exe,main,main)
