START=source ~/.platformio/penv/bin/activate
STOP=deactivate
all:
	$(START) && \
  platformio -c qtcreator run &&\
	$(STOP)

# regenerate project files to reflect platformio.ini changes
project-update:
	@echo "This will overwrite project metadata files.  Are you sure? [y/N] " \
  && read ans && [ $${ans:-'N'} = 'y' ]
	$(START) && \
  platformio project init --ide qtcreator -e alga_farm_baremetal_debug &&\
	$(STOP)


# forward any other target (clean, build, etc.) to pio run
%:
	$(START) && \
  platformio -c qtcreator run -e alga_farm_baremetal_debug --target $* &&\
	$(STOP)
  
