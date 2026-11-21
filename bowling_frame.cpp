#include "bowling_frame.h"
#include <random>

using namespace std;
//pins that are knocked down in one roll
static int random_roll(int pins_left){
	static random_device dev;
	static mt19937 rng(dev());
	uniform_int_distribution<int> distribution(0, pins_left);
	return distribution(rng);
}
Bowling_Frame::Bowling_Frame(int fnum)
	: score1_(0), score2_(0), strike_(false), spare_(false),
      roll_one_(false), roll_two_(false), total_score_(0),
      frame_num_(fnum), frame_score_(0), bonus_roll1_(0),
      bonus_roll2_(0), bonus_count_(0), next_frame_(nullptr),
      prev_frame_(nullptr) {}

Bowling_Frame::~Bowling_Frame()
{
}

Bowling_Frame* Bowling_Frame::next_frame() { return next_frame_; }
void Bowling_Frame::next_frame( Bowling_Frame * new_frame ) { next_frame_ = new_frame; }

Bowling_Frame* Bowling_Frame::prev_frame() { return prev_frame_; }
void Bowling_Frame::prev_frame( Bowling_Frame * new_frame ) { prev_frame_ = new_frame; }


int Bowling_Frame::score1() { return score1_; }

void Bowling_Frame::score1(int score) {
    score1_ = score;
    roll_one_ = true;
    strike_ = (score == 10);

    if (strike_) {
        score2_ = 0;
        roll_two_ = false;
        spare_ = false;
    }

    recalculate_scores();
}

int Bowling_Frame::score2() { return score2_; }

void Bowling_Frame::score2(int score) {
    score2_ = score;
    roll_two_ = true;
    spare_ = !strike_ && score1_ + score2_ == 10;
    recalculate_scores();
}


bool Bowling_Frame::strike() { return strike_; }
void Bowling_Frame::strike(bool value) {
    strike_ = value;
    recalculate_scores();
}

bool Bowling_Frame::spare() { return spare_; }
void Bowling_Frame::spare(bool value) {
    spare_ = value;
    recalculate_scores();
}

int Bowling_Frame::frame_num() { return frame_num_; }
int Bowling_Frame::frame_score() { return frame_score_; }

void Bowling_Frame::recalculate_scores() {
    Bowling_Frame* first = this;
    while (first->prev_frame_)
        first = first->prev_frame_;

    for (Bowling_Frame* frame = first; frame; frame = frame->next_frame_) {
        int points = frame->score1_ + frame->score2_;

        if (frame->frame_num_ == 10) {
            if (frame->bonus_count_ >= 1) points += frame->bonus_roll1_;
            if (frame->bonus_count_ >= 2) points += frame->bonus_roll2_;
        } else if (frame->strike_ || frame->spare_) {
            int rolls_needed = frame->strike_ ? 2 : 1;

            for (Bowling_Frame* next = frame->next_frame_;
                 next && rolls_needed > 0; next = next->next_frame_) {
                if (next->roll_one_) {
                    points += next->score1_;
                    --rolls_needed;
                }
                if (next->roll_two_ && rolls_needed > 0) {
                    points += next->score2_;
                    --rolls_needed;
                }
                if (next->frame_num_ == 10) {
                    if (next->bonus_count_ >= 1 && rolls_needed > 0) {
                        points += next->bonus_roll1_;
                        --rolls_needed;
                    }
                    if (next->bonus_count_ >= 2 && rolls_needed > 0) {
                        points += next->bonus_roll2_;
                        --rolls_needed;
                    }
                }
            }
        }
        frame->frame_score_ = points;
    }
}

void Bowling_Frame::update_scores() {
    if (!roll_one_) {
        score1_ = random_roll(10);
        roll_one_ = true;
        strike_ = score1_ == 10;
    } else if (!strike_ && !roll_two_) {
        score2_ = random_roll(10 - score1_);
        roll_two_ = true;
        spare_ = score1_ + score2_ == 10;
    } else if (frame_num_ == 10 && (strike_ || spare_) &&
               bonus_count_ < (strike_ ? 2 : 1)) {
        if (bonus_count_ == 0) {
            bonus_roll1_ = random_roll(10);
        } else {
            // If the first extra roll wasn't a strike, pins remain down.
            int pins_left = bonus_roll1_ == 10 ? 10 : 10 - bonus_roll1_;
            bonus_roll2_ = random_roll(pins_left);
        }
        ++bonus_count_;
    } else if (next_frame_) {
        next_frame_->update_scores();
    }

    recalculate_scores();
}
