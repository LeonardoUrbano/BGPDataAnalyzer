CPPFLAGS=-g -std=c++0x -Wall -Wextra
BZIP2_LD=-lbz2
GZIP_LD=-lz
PTHREAD_LD=-lpthread
CURL_LD=-lcurl
BOOST_LD=-lboost_system

example_NAME := BgpUpd_Downloader_and_Sender
example_C_SRCS := BgpUpd_Downloader_and_Sender/src/BgpUpd_Downloader_and_Sender.cpp \
BgpUpd_Downloader_and_Sender/src/Downloader.cpp \
BgpUpd_Downloader_and_Sender/src/Threads/DownloaderThreads.cpp \
BgpUpd_Downloader_and_Sender/src/Threads/FlowCreateThread.cpp \
BgpUpd_Downloader_and_Sender/src/Threads/SenderThread.cpp \
BgpUpd_Downloader_and_Sender/src/RouteCollectors/IsolarioRC.cpp \
BgpUpd_Downloader_and_Sender/src/RouteCollectors/RisRC.cpp \
BgpUpd_Downloader_and_Sender/src/RouteCollectors/RouteCollector.cpp \
BgpUpd_Downloader_and_Sender/src/RouteCollectors/RouteViewRC.cpp \
Common_files/BGP/BGP.cpp \
Common_files/BGP/BGP_RIB_snapshot.cpp \
Common_files/BGP/BGP_update.cpp \
Common_files/bitarray.cpp \
Common_files/bzip2.cpp \
Common_files/Components/Isolario_component.cpp \
Common_files/Components/Isolario_control_component.cpp \
Common_files/Core.cpp \
Common_files/gzip.cpp \
Common_files/IP.cpp \
Common_files/Log.cpp \
Common_files/MRT/MRT.cpp \
Common_files/MRT/MRT_BGP4MP_MESSAGE.cpp \
Common_files/MRT/MRT_BGP4MP_MESSAGE_AS4.cpp \
Common_files/MRT/MRT_BGP4MP_STATE_CHANGE_AS4.cpp \
Common_files/MRT/MRT_BGP4MP_STATE_CHANGE.cpp \
Common_files/MRT/MRT_parser.cpp \
Common_files/MRT/MRT_PEER_INDEX_TABLE.cpp \
Common_files/MRT/MRT_RIB_IPV6.cpp \
Common_files/MRT/MRT_RIB_IPV4.cpp \
Common_files/MRT/MRT_TABLE_DUMP.cpp \
Common_files/MRT/Peer_entry.cpp \
Common_files/MRT/RIB_entry.cpp \
Common_files/Socket.cpp \
Common_files/utils.cpp \
Common_files/Utils/BigInteger/BigUnsigned.cpp \
Common_files/Utils/BigInteger/BigUnsignedInABase.cpp \
Common_files/Utils/Commands.cpp \
Common_files/Utils/Date.cpp \
Common_files/Utils/Files.cpp \
Common_files/Utils/IP.cpp \
Common_files/Utils/Maths.cpp \
Common_files/Messages/Control_messages.cpp \
Common_files/Messages/Messages.cpp \
Common_files/Utils/Parser.cpp \
Common_files/Utils/String.cpp \
Geolocator/src/Geolocator_core.cpp \
Geolocator/src/Geolocator_threads.cpp \
Geolocator/src/Geolocator_block.cpp \
MRT_data_reader/src/MDR_core.cpp

example_C_OBJS := ${example_C_SRCS:.cpp=.o}
example_H := ${example_C_SRCS:.cpp=.o}
example_OBJS := $(example_C_OBJS)

$(example_NAME): $(example_OBJS)
	$(CXX) $(example_OBJS) -o $(example_NAME)/bin/$(example_NAME) $(PTHREAD_LD) $(DATE) $(GZIP_LD) $(BZIP2_LD) $(CURL_LD) $(BOOST_LD)

clean:
	find ./ -name "*.[oa]" -o -name "*~" | xargs rm -f
