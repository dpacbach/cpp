ifndef root
    include $(dir $(lastword $(MAKEFILE_LIST)))../Makefile
else
    # Must enter in order of dependencies.
    locations := pugixml # Put any additional sublocations here
    $(call enter_all,$(locations))

    $(call make_ar,xml,xml)
endif
