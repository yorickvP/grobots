// GBSideView.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBScoresView.h"
#include "GBSide.h"
#include "GBWorld.h"
#include "GBRobotType.h"
#include "GBStringUtilities.h"
#include <algorithm>

const short kColumnWidth = 95;
const short kGraphTop = 15;
const short kGraphWidth = kColumnWidth * 3 + kEdgeSpace * 2;
const short kTableHeight = 113;

void GBScoresView::DrawIncome(const GBScores & scores, const GBRect & box) {
	const GBIncomeStatistics & income = scores.Income();
	long total = income.Total();
	if ( ! total || ! scores.Rounds() ) return;
	DrawBox(box);
	DrawStringLongPair("Income:", total / scores.Rounds(), box, 10, 9, GBColor::black, true);
	DrawStringPair("Solar:", ToPercentString(income.Autotrophy(), total), box, 20, 9, GBColor::darkGreen);
	DrawStringPair("Manna:", ToPercentString(income.Theotrophy(), total), box, 30, 9, GBColor::darkGreen);
	DrawStringPair("Enemies:", ToPercentString(income.Heterotrophy(), total), box, 40, 9, GBColor::purple);
	DrawStringPair("Stolen:", ToPercentString(income.Kleptotrophy(), total), box, 50, 9, GBColor(0.4f, 0.6f, 0));
	DrawStringPair("Cannibal:", ToPercentString(income.Cannibalism(), total), box, 60, 9, GBColor::darkRed);
}

void GBScoresView::DrawExpenditures(const GBScores & scores, const GBRect & box) {
	const GBExpenditureStatistics & spent = scores.Expenditure();
	long total = spent.Total();
	if ( ! total || ! scores.Rounds() ) return;
	DrawBox(box);
	DrawStringLongPair("Spent:", total / scores.Rounds(), box, 10, 9, GBColor::black, true);
	DrawStringPair("Growth:", ToPercentString(spent.Construction(), total), box, 20, 9, GBColor::darkGreen);
	DrawStringPair("Engine:", ToPercentString(spent.Engine(), total), box, 30, 9, GBColor::black);
	DrawStringPair("Sensors:", ToPercentString(spent.Sensors(), total), box, 40, 9, GBColor::blue);
	DrawStringPair("Weapons:", ToPercentString(spent.Weapons(), total), box, 50, 9, GBColor::purple);
	DrawStringPair("Force:", ToPercentString(spent.ForceField(), total), box, 60, 9, GBColor::blue);
	if ( spent.Shield() )
		DrawStringPair("Shield:", ToPercentString(spent.Shield(), total), box, 70, 9, GBColor::gray);
	DrawStringPair("Repairs:", ToPercentString(spent.Repairs(), total), box, 80, 9, GBColor::black);
	DrawStringPair("Brains:", ToPercentString(spent.Brain(), total), box, 90, 9, GBColor::black);
	DrawStringPair("Stolen:", ToPercentString(spent.Stolen(), total), box, 100, 9, GBColor(0.4f, 0.6f, 0));
	DrawStringPair("Overflow:", ToPercentString(spent.Wasted(), total), box, 110, 9,
				   spent.Wasted() > 0.01 * total ? GBColor::darkRed : GBColor::gray);
}

void GBScoresView::DrawDeaths(const GBScores & scores, const GBRect & box) {
	DrawBox(box);
	DrawStringPair("Kill rate:", ToPercentString(scores.KillRate(), 0), box, 10, 9, GBColor::black);
	DrawStringLongPair("Kills:", scores.Killed(), box, 20, 9, GBColor::purple);
	DrawStringLongPair("Losses:", scores.Dead(), box, 30, 9, GBColor::black);
	DrawStringLongPair("Suicides:", scores.Suicide(), box, 40, 9, scores.Suicide() ? GBColor::darkRed : GBColor::gray);
}

void GBScoresView::DrawGraph(const GBRect & box, long vscale, int hscale,
							 const std::vector<long> & hist, const GBColor & color, short weight) {
	int n = hist.size() - 1;
//draw lines
	for ( int i = 0; i < n; ++ i )
		DrawLine(box.left + box.Width() * i / hscale,
			box.bottom - hist[i] * box.Height() / vscale,
			box.left + box.Width() * (i + 1) / hscale,
			box.bottom - hist[i + 1] * box.Height() / vscale,
			color, weight);
}

void GBScoresView::DrawGraph(const GBRect & box, bool allRounds) {
	if ( ! world.CountSides() ) return;
	DrawBox(box);
	GBRect graph(box.left + 1, box.top + 1, box.right - 2, box.bottom - 2);
	const GBSide * side = world.SelectedSide();
//find scale
	long scale = 1;
	int hscale = 1;
	const std::vector<GBSide *> & sides = world.Sides();
	for ( int i = 0; i < sides.size(); ++ i ) {
		GBSide * s = sides[i];
		if ( (allRounds ? s->TournamentScores().Rounds() : s->Scores().Rounds()) == 0 )
			continue;
		const std::vector<long> & hist = allRounds ? s->TournamentScores().BiomassHistory() : s->Scores().BiomassHistory();
		scale = max(*std::max_element(hist.begin(), hist.end()), scale);
		hscale = max((int)hist.size() - 1, hscale);
	}
	if ( hscale < 1 ) return;
//draw gridlines
	for ( int t = 45; t < hscale; t += 45 ) {
		short x = graph.left + t * graph.Width() / hscale;
		DrawLine(x, graph.bottom, x, graph.top, GBColor::lightGray);
	}
	for ( int quantum = 1000; quantum < scale; quantum *= 10 ) {
		if ( quantum < scale / 40 ) continue;
		for ( int en = quantum; en < scale; en += quantum ) {
			short y = graph.bottom - en * graph.Height() / scale;
			DrawLine(graph.left, y, graph.right, y, GBColor(0.98f - 0.4f * quantum / scale));
		}
	}
//draw curves
	for ( int i = 0; i < sides.size(); ++ i ) {
		GBSide * s = sides[i];
		if ( (allRounds ? s->TournamentScores().Rounds() : s->Scores().Rounds()) == 0 )
			continue;
		const std::vector<long> & hist = (allRounds ? s->TournamentScores().BiomassHistory() : s->Scores().BiomassHistory());
		DrawGraph(graph, scale, hscale, hist, s->Color().ContrastingTextColor(), s == side ? 2 : 1);
	}
	DrawStringLeft(ToString(scale), box.left + 3, box.top + 10, 9, GBColor::gray);
	DrawStringRight(ToString(hscale * 100), box.right - 3, box.bottom - 3, 9, GBColor::gray);
	DrawOpenRect(box, GBColor::black); //clean up spills
}

void GBScoresView::DrawScores(const GBScores & scores, bool allRounds) {
	short left = allRounds ? kEdgeSpace * 2 + kGraphWidth : kEdgeSpace;
	short top = Height() - kTableHeight - kEdgeSpace;
	const GBSide * side = world.SelectedSide();
//caption
	const string & title = allRounds ? "Average over " + ToString(scores.Rounds()) + (scores.Rounds() == 1 ? " round" : " rounds")
									 : side ? side->Name() + " this round" : "This round";
	DrawStringLeft(title + ":", left + 3, kGraphTop - 4, 10);
	if ( ! scores.Rounds() ) {
		DrawStringCentered(allRounds ? "no data" : "not seeded", left + kGraphWidth / 2, top + kTableHeight / 2, 10);
		return;
	}
	GBRect col(left, top, left + kColumnWidth, Height() - kEdgeSpace);
//income (6 lines)
	col.bottom = top + 65;
	DrawIncome(scores, col);
//deaths (4 lines)
	col.top = col.bottom + kEdgeSpace;
	col.bottom = Height() - kEdgeSpace;
	DrawDeaths(scores, col);
//expenditures (11 lines)
	col.top = top;
	col.left += kColumnWidth + kEdgeSpace;
	col.right += kColumnWidth + kEdgeSpace;
	DrawExpenditures(scores, col);
//other stats (5 lines)
	col.left += kColumnWidth + kEdgeSpace;
	col.right += kColumnWidth + kEdgeSpace;
	DrawBox(col);
	DrawStringLongPair("Biomass:", scores.Biomass(), col, 10, 9, GBColor::black, true);
	if ( allRounds ) {
		DrawStringLongPair("Early:", scores.EarlyBiomass(), col, 20, 9, GBColor::darkGreen);
		DrawStringPair("Survival:", ToPercentString(scores.Survival()), col, 30, 9, GBColor::black);
		DrawStringPair("Early death:", ToPercentString(scores.EarlyDeathRate(), 0), col, 40, 9, GBColor::black);
		DrawStringPair("Late death:", ToPercentString(scores.LateDeathRate(), 0), col, 50, 9, GBColor::black);
	} else { // current stats
		DrawStringLongPair("Population:", scores.Population(), col, 20, 9, GBColor::blue);
		DrawStringLongPair("Ever:", scores.PopulationEver(), col, 30, 9, GBColor::blue);
		if ( side ) {
			if ( scores.Sterile() && side->Scores().SterileTime() != side->Scores().ExtinctTime() )
				DrawStringLongPair("Sterile:", side->Scores().SterileTime(), col, 40, 9, GBColor::purple);
			if ( ! scores.Population() )
				DrawStringLongPair("Extinct:", side->Scores().ExtinctTime(), col, 50, 9, GBColor::red);
		} else {
			DrawStringLongPair("Manna:", world.MannaValue(), col, 40, 9, GBColor::darkGreen);
			DrawStringLongPair("Corpses:", world.CorpseValue(), col, 50, 9, GBColor::red);
		}
	}
//growth (3 lines)
	DrawStringLongPair("Seeded:", scores.Seeded(), col, 60, 9, GBColor::black);
	if ( scores.Efficiency() > 0 )
		DrawStringPair("Efficiency:", ToPercentString(scores.Efficiency(), 0),
					   col, 70, 9, GBColor::black);
	long doubletime = scores.Doubletime(world.CurrentFrame());
	if ( ! allRounds && doubletime && abs((int)doubletime) < 1000000 )
		DrawStringLongPair("Doubletime:", doubletime, col, 80, 9, GBColor::black);
//other
	if ( scores.Population() ) {
		DrawStringPair("Economy:", ToPercentString(scores.EconFraction(), 0), col, 90, 9, GBColor::black);
		DrawStringPair("Combat:", ToPercentString(scores.CombatFraction(), 0), col, 100, 9, GBColor::black);
	}
	if ( side )
		DrawStringLongPair("Territory:", scores.Territory(), col, 110, 9, GBColor::black);
}

void GBScoresView::Draw() {
	const GBSide * side = world.SelectedSide();
	DrawBackground();
//tables
	DrawScores(side ? side->Scores() : world.RoundScores(), false);
	DrawScores(side ? side->TournamentScores() : world.TournamentScores(), true);
//drawing graphs last to reduce flicker
	GBRect graphbox(kEdgeSpace, kGraphTop, kEdgeSpace + kGraphWidth, Height() - kTableHeight - kEdgeSpace * 2);
	DrawGraph(graphbox, false);
	graphbox.left = graphbox.right + kEdgeSpace;
	graphbox.right = graphbox.left + kGraphWidth;
	if ( world.TournamentScores().Rounds() )
		DrawGraph(graphbox, true);
// record
	lastDrawnWorld = world.ChangeCount();
	lastSideDrawn = world.SelectedSide();
}

GBScoresView::GBScoresView(GBWorld & w)
	: world(w),
	lastDrawnWorld(-1), lastSideDrawn(nil)
{}

GBMilliseconds GBScoresView::RedrawInterval() const {
	return 2000;
}

bool GBScoresView::InstantChanges() const {
	return lastSideDrawn != world.SelectedSide();
}

bool GBScoresView::DelayedChanges() const {
	return lastDrawnWorld != world.ChangeCount();
}

short GBScoresView::PreferredWidth() const {
	return kColumnWidth * 6 + kEdgeSpace * 7;
}

const string GBScoresView::Name() const {
	return "Statistics";
}

short GBScoresView::PreferredHeight() const {
	return 300;
}

