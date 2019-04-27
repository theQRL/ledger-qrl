#*******************************************************************************
#*   (c) 2019 ZondaX GmbH
#*
#*  Licensed under the Apache License, Version 2.0 (the "License");
#*  you may not use this file except in compliance with the License.
#*  You may obtain a copy of the License at
#*
#*      http://www.apache.org/licenses/LICENSE-2.0
#*
#*  Unless required by applicable law or agreed to in writing, software
#*  distributed under the License is distributed on an "AS IS" BASIS,
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*  See the License for the specific language governing permissions and
#*  limitations under the License.
#********************************************************************************

.PHONY: all deps build clean load delete

LEDGER_SRC=$(CURDIR)/src/ledger
DOCKER_IMAGE=zondax/ledger-docker-bolos
DOCKER_BOLOS_SDK=/project/deps/nanos-secure-sdk

all: build

deps:
	@echo "Install dependencies"
	$(CURDIR)/src/install_deps.sh

build:
	docker run -i --rm \
	-e BOLOS_SDK=$(DOCKER_BOLOS_SDK) -e BOLOS_ENV=/opt/bolos \
	-u $(shell id -u) -v $(shell pwd):/project \
	$(DOCKER_IMAGE) \
	make -C /project/src/ledger

clean:
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_SRC) clean

load: build
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_SRC) load

delete:
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_SRC) delete
