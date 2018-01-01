ifndef root
    include $(dir $(lastword $(MAKEFILE_LIST)))../Makefile
else
    # Must enter in order of dependencies.
    locations := sqlite-amalgamation-3210000 c++
    $(call enter_all,$(locations))

    $(call make_ar,sqlite,sqlite)
endif
