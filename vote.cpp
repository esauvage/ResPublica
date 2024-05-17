#include "vote.h"

Vote::Vote() {}

void Vote::setQuestion(const std::shared_ptr<Question> &newQuestion)
{
    _question = newQuestion;
}
