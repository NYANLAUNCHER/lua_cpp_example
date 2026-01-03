.PHONY: default
default: sys_paths.h

.PHONY: sys_paths.h
sys_paths.h:
ifndef RESDEST
$(error Variable $$(RESDEST) must be defined)
endif
	@echo "#pragma once" > $@
	@echo "#define RESOURCE_DIR \"$(RESDEST)\"" >> $@
	@echo "#define SHADER_DIR RESOURCE_DIR\"/shaders\"" >> $@
	@echo "#define TEXTURE_DIR RESOURCE_DIR\"/textures\"" >> $@
	@echo "#define LUA_DIR RESOURCE_DIR\"/lua\"" >> $@
