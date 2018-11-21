# Path to the metrics plugin directory. The metric plugin API
# header files should be in the 'include/' subdirectory to this.
ifndef ALLINEA_METRIC_PLUGIN_DIR
$(error "Set ALLINEA_METRIC_PLUGIN_DIR to the Metrics SDK root directory, e.g. $$ALLINEA_FORGE_PATH/map/metrics")
endif

# Path to the MPWIDE install directory. 
# Header files should be in the 'include/' subdirectory to this.
# -I/opt/mpwide/include/ -L/opt/mpwide/lib/ -lmpwide
MPWIDE_HOME=$(PWD)/MPWide
ifndef MPWIDE_HOME
$(error "MPWIDE_HOME unset! Make sure to run e.g. 'source /opt/mpwide/etc/mpwide.profile' before running MPWIDE.")
endif

CC=gcc
IDIRS=-I ${ALLINEA_METRIC_PLUGIN_DIR}/include -I ${MPWIDE_HOME}
CFLAGS=-std=gnu99 -Wall -Werror -g
LFLAGS=-fPIC -shared -L${MPWIDE_HOME} -lMPW

.PHONY: all
all: libmpwide.so

libmpwide.so: libmpwide.c
	$(CC) $(CFLAGS) $< -o $@ $(IDIRS) $(LFLAGS)

.PHONY: install
install: libmpwide.so mpwide.xml
	if [ ! -d ~/.allinea/map/metrics ]; then mkdir -p ~/.allinea/map/metrics; fi
	cp -u libmpwide.so mpwide.xml ~/.allinea/map/metrics/
	if [ ! -d ~/.allinea/perf-report/reports/ ]; then mkdir -p ~/.allinea/perf-report/reports/; fi
	cp -u mpwide-report.xml ~/.allinea/perf-report/reports/

.PHONY: clean
clean:
	rm -f libmpwide.so
