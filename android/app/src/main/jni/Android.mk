LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := noveltea-launcher

LOCAL_SRC_FILES := main.cpp
LOCAL_STATIC_LIBRARIES := noveltea
LOCAL_LDLIBS := -lGLESv1_CM -lc
LOCAL_CFLAGS := -std=c++11 -fexceptions -frtti

include $(BUILD_SHARED_LIBRARY)

$(call import-module,noveltea)
