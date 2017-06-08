
SPECS = ast.vpp basetools.vpp batcherr.vpp cgrepos.vpp \
        colleague.vpp errors.vpp errstate.vpp filestatus.vpp \
        interface.vpp mediator.vpp nonprinterr.vpp options.vpp \
        prompterr.vpp ptypes.vpp repository.vpp scripterr.vpp \
        statestore.vpp toolkit.vpp unit.vpp unitstatus.vpp \
	updateses.vpp noneses.vpp structses.vpp flatses.vpp \
	updateproj.vpp statusinfo.vpp repdb.vpp sabasetools.vpp \
        depend.vpp

SPECDIR = spec
LIBDIR  = specman

VPPFILES = $(patsubst %, ../$(SPECDIR)/%, $(SPECS))

link:
	-mkdir $(LIBDIR)
	(cd $(LIBDIR); ../mkStatus $(VPPFILES); ln -s ../CGinfo)
	clvpp `cat ./.tmparg`
	rm -f ./.tmparg

gencode/AST.cc:                spec/ast.vpp        
	@echo Code Generate $(@:gencode/%.cc=%)
gencode/BaseTools.cc:	       spec/basetools.vpp  
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/BatchErr.cc:	       spec/batcherr.vpp   
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/CGRepository.cc:       spec/cgrepos.vpp    
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/ToolColleague.cc:      spec/colleague.vpp  
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/Errors.cc:	       spec/errors.vpp     
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/ErrorState.cc:	       spec/errstate.vpp   
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/FileStatus.cc:	       spec/filestatus.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/FlatSes.cc:	       spec/flatses.vpp    
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/Interface.cc:	       spec/interface.vpp  
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/ToolMediator.cc:       spec/mediator.vpp   
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/NoneSes.cc:	       spec/noneses.vpp    
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/NonPrintErr.cc:	       spec/nonprinterr.vpp
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/Options.cc:	       spec/options.vpp    
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/PromptErr.cc:	       spec/prompterr.vpp  
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/ProjectTypes.cc:       spec/ptypes.vpp     
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/Repository.cc:	       spec/repository.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/ScriptErr.cc:	       spec/scripterr.vpp  
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/StateStore.cc:	       spec/statestore.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/StatusInfo.cc:	       spec/statusinfo.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/StructSes.cc:	       spec/structses.vpp  
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/ToolKit.cc:	       spec/toolkit.vpp    
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/VDMUnit.cc:	       spec/unit.vpp       
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/UnitStatus.cc:	       spec/unitstatus.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/UpdateProject.cc:      spec/updateproj.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/UpdateSes.cc:          spec/updateses.vpp
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
#	clvpp -l $(LIBDIR) -g $(patsubst gencode/%.cc, %, $@)
gencode/RepDatabase.cc:      spec/repdb.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)
gencode/Dependency.cc:      spec/depend.vpp 
	@echo Code Generate $(patsubst gencode/%.cc, %, $@)




cg:    gencode/AST.cc \
       gencode/BaseTools.cc \
       gencode/BatchErr.cc \
       gencode/CGRepository.cc \
       gencode/ToolColleague.cc \
       gencode/Errors.cc \
       gencode/ErrorState.cc \
       gencode/FileStatus.cc \
       gencode/FlatSes.cc \
       gencode/Interface.cc \
       gencode/ToolMediator.cc \
       gencode/NoneSes.cc \
       gencode/NonPrintErr.cc \
       gencode/Options.cc \
       gencode/PromptErr.cc \
       gencode/ProjectTypes.cc \
       gencode/Repository.cc \
       gencode/ScriptErr.cc \
       gencode/StateStore.cc \
       gencode/StatusInfo.cc \
       gencode/StructSes.cc \
       gencode/ToolKit.cc \
       gencode/VDMUnit.cc \
       gencode/UnitStatus.cc \
       gencode/UpdateProject.cc \
       gencode/UpdateSes.cc  \
       gencode/RepDatabase.cc  gencode/SABaseTools.cc   gencode/Dependency.cc

