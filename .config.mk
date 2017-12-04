CXXFLAGS += -std=c++1z

LDFLAGS += -lstdc++fs

no_top_bin_folder = 1

main.deps      = xml-utils project
project.deps   = xml-utils
xml-utils.deps = pugixml util

main_is = main

$(call enter_all,util xml-utils project)
$(call make_exe,main,main$(opt-suffix))
