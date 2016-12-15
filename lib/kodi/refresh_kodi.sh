DIR=$(dirname $0)
KODI_PVR=$DIR/pvr

BASE_URL=https://raw.githubusercontent.com/xbmc/xbmc/Jarvis/
OPTIONS=""
curl $OPTIONS $BASE_URL/xbmc/cores/dvdplayer/DVDDemuxers/DVDDemuxPacket.h > $KODI_PVR/DVDDemuxPacket.h
curl $OPTIONS $BASE_URL/addons/library.xbmc.pvr/libXBMC_pvr.h > $KODI_PVR/libXBMC_pvr.h
curl $OPTIONS $BASE_URL/addons/library.xbmc.codec/libXBMC_codec.h > $KODI_PVR/libXBMC_codec.h
curl $OPTIONS $BASE_URL/addons/library.kodi.guilib/libKODI_guilib.h > $KODI_PVR/libKODI_guilib.h
curl $OPTIONS $BASE_URL/addons/library.xbmc.addon/libXBMC_addon.h > $KODI_PVR/libXBMC_addon.h
curl $OPTIONS $BASE_URL/addons/library.xbmc.addon/dlfcn-win32.h > $KODI_PVR/dlfcn-win32.h
curl $OPTIONS $BASE_URL/addons/library.xbmc.addon/dlfcn-win32.cpp > $KODI_PVR/dlfcn-win32.cpp
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_addon_cpp_dll.h > $KODI_PVR/xbmc_addon_cpp_dll.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_addon_dll.h > $KODI_PVR/xbmc_addon_dll.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_addon_types.h > $KODI_PVR/xbmc_addon_types.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_codec_types.h > $KODI_PVR/xbmc_codec_types.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_epg_types.h > $KODI_PVR/xbmc_epg_types.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_pvr_dll.h > $KODI_PVR/xbmc_pvr_dll.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_pvr_types.h > $KODI_PVR/xbmc_pvr_types.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_scr_dll.h > $KODI_PVR/xbmc_scr_dll.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_scr_types.h > $KODI_PVR/xbmc_scr_types.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_vis_dll.h > $KODI_PVR/xbmc_vis_dll.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_vis_types.h > $KODI_PVR/xbmc_vis_types.h
curl $OPTIONS $BASE_URL/xbmc/addons/include/xbmc_stream_utils.hpp > $KODI_PVR/xbmc_stream_utils.hpp

