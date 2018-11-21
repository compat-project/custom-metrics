# Compile and install the example custom metrics plugin along with the partial report.

# Path to the metrics plugin directory. The metric plugin API
# header files should be in the 'include/' subdirectory to this.

ifndef ALLINEA_METRIC_PLUGIN_DIR
$(error "Set ALLINEA_METRIC_PLUGIN_DIR to the Metrics SDK root directory, e.g. $$ALLINEA_FORGE_PATH/map/metrics")
endif

CC=gcc
CFLAGS=-fPIC -I$(ALLINEA_METRIC_PLUGIN_DIR)/include
LFLAGS=
DEFAULTCONFIGDIR=~/.allinea

CONFIGDIR = $(ALLINEA_CONFIG_DIR)
ifndef ALLINEA_CONFIG_DIR
    CONFIGDIR = $(DEFAULTCONFIGDIR)
endif

MAPCONFIGDIR = $(CONFIGDIR)/map/metrics
PERFCONFIGDIR = $(CONFIGDIR)/perf-report/reports

.PHONY: all
all: libsmucenergy.so
	@echo "Use 'make install' to install the metric to $(MAPCONFIGDIR) and the partial report to $(PERFCONFIGDIR) for testing."

libsmucenergy.so: SMUCEnergy.c
	$(CC) $(CFLAGS) -shared -o $@ $< $(LFLAGS)


.PHONY: install-metric
install-metric: libsmucenergy.so SMUCEnergy.xml
	cp $^ $(MAPCONFIGDIR)

.PHONY: install-partial
install-partial: SMUCEnergy_report.xml
	cp $^ $(PERFCONFIGDIR)

$(CONFIGDIR)/%:
	mkdir -p $@

.PHONY: install
install: $(MAPCONFIGDIR) $(PERFCONFIGDIR) install-metric install-partial

.PHONY: clean
clean:
	-rm *.so
