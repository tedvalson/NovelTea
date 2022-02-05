#include <NovelTea/Engine.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/Settings.hpp>
#include <NovelTea/TextInput.hpp>
#include <SFML/System/NativeActivity.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <android/native_activity.h>

namespace {
	bool gettingInput = false;
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
	jstring strClassName = env->NewStringUTF("com.noveltea.launcher.TextInputActivity");
	auto res = (jclass)(env->CallObjectMethod(cls, findClass, strClassName));
	env->DeleteLocalRef(strClassName);
	return res;
}

void triggerTextInput(const std::string &message, int ref)
{
	auto activity = sf::getNativeActivity();
	JavaVM* vm = activity->vm;
	JNIEnv* env;
	if (attachThread(&env) == JNI_ERR)
		return;

	jclass c = fetchClass(env);
	jobject na = activity->clazz;
	jstring s = env->NewStringUTF(message.c_str());
	jmethodID m = env->GetStaticMethodID(c, "launch", "(Ljava/lang/String;ILandroid/app/NativeActivity;)V");
	env->CallStaticObjectMethod(c, m, s, ref, na);
	
	detachThread();
	gettingInput = true;
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
	auto nativeActivity = sf::getNativeActivity();
	
	GTextInput.textInputTrigger = triggerTextInput;
	
	GSettings.setDirectory(nativeActivity->internalDataPath);
	GSettings.load();

	sf::VideoMode screen(sf::VideoMode::getDesktopMode());

	sf::RenderWindow window(screen, "");
	window.setFramerateLimit(30);
	
	NovelTea::EngineConfig config;
	config.width = window.getSize().x;
	config.height = window.getSize().y;
	config.fontSizeMultiplier = GSettings.getFontSizeMultiplier();
	config.fps = 30;
	config.initialState = NovelTea::StateID::Intro;
	config.saveDir = nativeActivity->internalDataPath;
	
	auto engine = new NovelTea::Engine(config);
	engine->initialize();

	GSave->setDirectory(nativeActivity->internalDataPath);

	std::string projDir = getenv("EXTERNAL_STORAGE");
	projDir += "/test";
	Proj.loadFromFile(projDir + "/test.ntp");
	

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
	}
	
	delete engine;
	return EXIT_SUCCESS;
}
