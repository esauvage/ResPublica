#ifndef VOTE_H
#define VOTE_H

#include "question.h"

class Vote
{
public:
    Vote();
    void setQuestion(const std::shared_ptr<Question> &newQuestion);

private:
    std::shared_ptr<Question> _question;
};

#endif // VOTE_H
