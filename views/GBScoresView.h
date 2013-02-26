// GBScoresView.h
// scores and statistics
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef GBScoresView_h
#define GBScoresView_h

#include "GBListView.h"
#include "GBWorld.h"


class GBScoresView : public GBView {
	GBWorld & world;
	GBChangeCount lastDrawnWorld;
	const GBSide * lastSideDrawn;
	
	void DrawIncome(const GBScores & scores, const GBRect & box);
	void DrawExpenditures(const GBScores & scores, const GBRect & box);
	void DrawDeaths(const GBScores & scores, const GBRect & box);
	void DrawGraph(const GBRect & box, long vscale, int hscale,
		const std::vector<long> & hist, const GBColor & color, short weight);
	void DrawGraph(const GBRect & box, bool allRounds);
	void DrawScores(const GBScores & scores, bool allRounds);
public:
	explicit GBScoresView(GBWorld & rost);

	void Draw();
	
	GBMilliseconds RedrawInterval() const;
	bool InstantChanges() const;
	bool DelayedChanges() const;
	
	short PreferredWidth() const;
	short PreferredHeight() const;
	
	const string Name() const;
};

#endif
