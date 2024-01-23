#ifndef NOVELTEA_DIALOGUEPLAYER_HPP
#define NOVELTEA_DIALOGUEPLAYER_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/TextTypes.hpp>
#include <NovelTea/json.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace NovelTea
{

class Dialogue;
class DialogueSegment;

using DialogueCallback = std::function<void(int)>;

struct DialogueOption {
	std::string text;
	bool enabled;
	std::function<void()> exec;
};

class DialoguePlayer : public ContextObject
{
public:
	DialoguePlayer(Context *context);

	void setDialogue(const std::shared_ptr<Dialogue> &dialogue);
	const std::shared_ptr<Dialogue> &getDialogue() const;
	void reset();
	void update(float delta);

	void processLines();
	bool processSelection(int buttonIndex);
	void setDialogueCallback(DialogueCallback callback);

	void changeSegment(int newSegmentIndex, bool run = true, int buttonSubindex = -1);
	void changeLine(int newLineIndex);
	bool continueToNext();

	sj::JSON saveState() const;
	void restoreState(const sj::JSON &jstate);

	bool isComplete() const;

protected:
	void genOptions(const std::shared_ptr<DialogueSegment> &parentNode, bool isRoot);

	void repositionButtons(float fontSize);

private:
	std::shared_ptr<Dialogue> m_dialogue;
	DialogueCallback m_callback;
	int m_currentSegmentIndex;
	int m_nextForcedSegmentIndex;
	bool m_isComplete;
	bool m_isShowing;
	bool m_logCurrentIndex;

	int m_textLineIndex;
	std::vector<std::pair<std::string,std::string>> m_textLines;

	std::vector<std::unique_ptr<DialogueOption>> m_options;

	AnimationProperties m_animProps;
	AnimationProperties m_animNameProps;
};

} // namespace NovelTea

#endif // NOVELTEA_DIALOGUEPLAYER_HPP
