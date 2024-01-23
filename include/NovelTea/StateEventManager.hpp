#ifndef NOVELTEA_STATE_EVENT_MANAGER_HPP
#define NOVELTEA_STATE_EVENT_MANAGER_HPP

#include <NovelTea/Event.hpp>
#include <dukglue/dukvalue.h>
#include <memory>

#define SEM GSys(StateEventManager)

namespace NovelTea
{

class Cutscene;
class CutscenePlayer;
class Dialogue;
class DialoguePlayer;

enum EntityMode {
	ModeNothing,
	ModeCutscene,
	ModeDialogue,
	ModeRoom,
};

namespace StateEvent
{
	enum Type {
		ModeChanged = Event::EventCount + 1,
		RoomTextChanged,
		DialogueChanged,
		Message,
		ScriptExecuted,

		// User response events
		DialogueContinue,
		DialogueChoice,
		MessageContinue,
	};

	struct RoomEvent {
		std::string text;
	};

	struct DialogueEvent: public Event {
		DialogueEvent(const std::string &name, const std::string &text, bool waitForClick)
			: Event(DialogueChanged), name(name), text(text), waitForClick(waitForClick)
			{}
		struct Option {
			std::string text;
			bool clickable;
			bool enabled;
		};
		std::vector<Option> options;
		std::string name;
		std::string text;
		bool waitForClick;
	};

	struct MessageEvent : public Event {
		MessageEvent(const std::vector<std::string> &messages)
			: Event(Message)
			, messages(messages)
			{}
		std::vector<std::string> messages;
	};
}

class StateEventManager : public Subsystem
{
public:
	StateEventManager(Context* context);
	virtual ~StateEventManager();

	static std::string name() { return "StateEventManager"; }

	bool initialize() override;
	void update(float delta) override;

	void setMode(EntityMode mode, const std::string &idName = std::string());
	void setMode(const sj::JSON &jEntity);

	bool gotoNextEntity();
	void updateRoomText();

	void move(int direction, const sj::JSON &jentity);

protected:
	void callOverlayFunc();

private:
	EntityMode m_mode;
	bool m_testPlaybackMode;
	bool m_testRecordMode;
	bool m_quitting;
	double m_playTime;

	DukValue m_textOverlayFunc;

	std::string m_roomText;

	// Cutscene
	std::shared_ptr<Cutscene> m_cutscene;
	std::shared_ptr<CutscenePlayer> m_cutscenePlayer;
	float m_cutsceneSpeed;

	// Dialogue
	std::shared_ptr<Dialogue> m_dialogue;
	std::shared_ptr<DialoguePlayer> m_dialoguePlayer;
};

} // namespace NovelTea

#endif // NOVELTEA_STATE_EVENT_MANAGER_HPP
