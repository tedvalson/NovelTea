#include <NovelTea/SFML/EngineSFML.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Settings.hpp>
#include <NovelTea/TextInput.hpp>
#include <SFML/System/NativeActivity.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <android/native_activity.h>
#include <android/log.h>

#define LOGV(...) { __android_log_print(ANDROID_LOG_INFO, "noveltea", __VA_ARGS__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); }
#define LOG(s) { LOGV("%s", s) }

namespace {
	bool gettingInput = false;
	JNIEnv* attachedEnv = NULL;
}

jint attachThread(JNIEnv** pEnv)
{
	// Attach this thread to the main thread
	JavaVMAttachArgs attachargs;
	attachargs.version = JNI_VERSION_1_6;
	attachargs.name = "NativeThread";
	attachargs.group = NULL;
	return sf::getNativeActivity()->vm->AttachCurrentThread(pEnv, &attachargs);
}

void detachThread()
{
	sf::getNativeActivity()->vm->DetachCurrentThread();
}

jclass fetchClass(JNIEnv* env)
{
	auto nativeActivity = sf::getNativeActivity();
	JavaVM* vm = nativeActivity->vm;
	
	jobject na = nativeActivity->clazz;
	jclass acl = env->GetObjectClass(na);
	jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = env->CallObjectMethod(na, getClassLoader);
	jclass classLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = env->NewStringUTF("com.noveltea.launcher.Helper");
	auto res = (jclass)(env->CallObjectMethod(cls, findClass, strClassName));
	env->DeleteLocalRef(strClassName);
	return res;
}

void triggerTextInput(const std::string &message, int ref)
{
	auto activity = sf::getNativeActivity();
	JavaVM* vm = activity->vm;
	if (!gettingInput) {
		gettingInput = true;
		if (attachThread(&attachedEnv) == JNI_ERR)
			return;
	}

	jclass c = fetchClass(attachedEnv);
	jobject na = activity->clazz;
	jstring s = attachedEnv->NewStringUTF(message.c_str());
	jmethodID m = attachedEnv->GetStaticMethodID(c, "launch", "(Ljava/lang/String;ILandroid/app/NativeActivity;)V");
	attachedEnv->CallStaticVoidMethod(c, m, s, ref, na);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_noveltea_launcher_TextInputActivity_showAlertCallback(JNIEnv *env, jclass thiz, jstring jstr, jint jref) {
	auto cs = env->GetStringUTFChars(jstr, 0);
	GTextInput.callback(cs, jref);
	env->ReleaseStringUTFChars(jstr, cs);
	if (GTextInput.finished())
		gettingInput = false;
}

int main(int argc, char *argv[])
{
	if (attachThread(&attachedEnv) == JNI_ERR)
		return EXIT_FAILURE;
		
	NovelTea::ContextConfig config;
	
	auto nativeActivity = sf::getNativeActivity();
	jclass helperClass = fetchClass(attachedEnv);
	jobject na = nativeActivity->clazz;
	
	// Get DPI so engine can adjust font size accordingly
	jmethodID methodID = attachedEnv->GetStaticMethodID(helperClass, "getDPI", "(Landroid/app/NativeActivity;)Landroid/util/DisplayMetrics;");
	jobject displayMetrics = attachedEnv->CallStaticObjectMethod(helperClass, methodID, na);
	jclass displayMetricsClass = attachedEnv->FindClass("android/util/DisplayMetrics");
	jfieldID xdpi_id = attachedEnv->GetFieldID(displayMetricsClass, "xdpi", "F");
	float xdpi = attachedEnv->GetFloatField(displayMetrics, xdpi_id);

	// Get project file name passed to activity's Intent
	methodID = attachedEnv->GetStaticMethodID(helperClass, "getProjectFileName", "(Landroid/app/NativeActivity;)Ljava/lang/String;");
	jstring jstr = static_cast<jstring>(attachedEnv->CallStaticObjectMethod(helperClass, methodID, na));
	if (jstr) {
		auto cs = attachedEnv->GetStringUTFChars(jstr, 0);
		if (cs)
			config.projectFileName = cs;
		attachedEnv->ReleaseStringUTFChars(jstr, cs);
	}
	
	GTextInput.textInputTrigger = triggerTextInput;

	sf::VideoMode screen(sf::VideoMode::getDesktopMode());
	sf::RenderWindow window(screen, "", sf::Style::Fullscreen);
	window.setFramerateLimit(30);
	
	config.width = window.getSize().x;
	config.height = window.getSize().y;
	config.dpiMultiplier = xdpi / 160.f;
	config.maxFps = 30;
	config.initialState = NovelTea::StateID::Intro;
	config.saveDir = nativeActivity->internalDataPath;
	config.settingsDir = nativeActivity->internalDataPath;
	if (config.projectFileName.empty()) // Skip intro
		config.projectFileName = "test.ntp";
	else
		config.initialState = NovelTea::StateID::TitleScreen;

	auto context = new NovelTea::Context(config);
	auto engine = new NovelTea::EngineSFML(context);
	engine->initialize();
	engine->setFramerateLocked(true);
	
	// We shouldn't try drawing to the screen while in background
	// so we'll have to track that. You can do minor background
	// work, but keep battery life in mind.
	bool active = true;

	while (window.isOpen())
	{
		sf::Event event;

		while (window.isOpen() && (active ? window.pollEvent(event) : window.waitEvent(event)))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				engine->resize(event.size.width, event.size.height);
			// On Android MouseLeft/MouseEntered are (for now) triggered,
			// whenever the app loses or gains focus.
			else if (event.type == sf::Event::MouseLeft || event.type == sf::Event::LostFocus){
				window.setActive(false);
				active = false;
			}else if (event.type == sf::Event::MouseEntered || event.type == sf::Event::GainedFocus){
				engine->update(0.f);
				window.setActive(true);
				active = true;
			}

			engine->processEvent(event);
		}
		
		if (active && !gettingInput)
		{
			engine->update();
			engine->render(window);
			window.display();
		}
		else
		{
			sf::sleep(sf::milliseconds(100));
			if (gettingInput) {
				engine->update(0.f);
			}
		}
		
		if (attachedEnv && !gettingInput) {
			attachedEnv = NULL;
			detachThread();
		}
	}

	return EXIT_SUCCESS;
}
