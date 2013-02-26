// GBScores.cpp
// Grobots (c) 2002-2006 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBScores.h"
#include <math.h>

const GBFrames kEarlyDeathTime = 4500;
const long kMaxSterileConstructor = 10;

// GBIncomeStatistics //

GBIncomeStatistics::GBIncomeStatistics()
	: autotrophy(0),
	theotrophy(0),
	heterotrophy(0),
	cannibalism(0),
	kleptotrophy(0)
{}

GBIncomeStatistics::~GBIncomeStatistics() {}

void GBIncomeStatistics::Reset() {
	autotrophy = 0;
	theotrophy = 0;
	heterotrophy = 0;
	cannibalism = 0;
	kleptotrophy = 0;
}

void GBIncomeStatistics::ReportAutotrophy(const GBEnergy en) { autotrophy += ToDouble(en);}
void GBIncomeStatistics::ReportTheotrophy(const GBEnergy en) { theotrophy += ToDouble(en);}
void GBIncomeStatistics::ReportHeterotrophy(const GBEnergy en) { heterotrophy += ToDouble(en);}
void GBIncomeStatistics::ReportCannibalism(const GBEnergy en) { cannibalism += ToDouble(en);}
void GBIncomeStatistics::ReportKleptotrophy(const GBEnergy en) { kleptotrophy += ToDouble(en);}

long GBIncomeStatistics::Autotrophy() const { return autotrophy; }
long GBIncomeStatistics::Theotrophy() const { return theotrophy; }
long GBIncomeStatistics::Heterotrophy() const { return heterotrophy; }
long GBIncomeStatistics::Cannibalism() const { return cannibalism; }
long GBIncomeStatistics::Kleptotrophy() const { return kleptotrophy; }

long GBIncomeStatistics::Total() const {
	// excludes cannibalism and seeded
	return Autotrophy() + Theotrophy() + Heterotrophy() + Kleptotrophy();
}

GBIncomeStatistics & GBIncomeStatistics::operator +=(const GBIncomeStatistics & other) {
	autotrophy += other.autotrophy;
	theotrophy += other.theotrophy;
	heterotrophy += other.heterotrophy;
	cannibalism += other.cannibalism;
	kleptotrophy += other.kleptotrophy;
	return *this;
}

// GBExpenditureStatistics //

GBExpenditureStatistics::GBExpenditureStatistics()
	: construction(0), engine(0),
	weapons(0), forceField(0), shield(0), repairs(0),
	sensors(0), brain(0), stolen(0), wasted(0)
{}

GBExpenditureStatistics::~GBExpenditureStatistics() {}

void GBExpenditureStatistics::Reset() {
	construction = 0;
	engine = 0;
	weapons = 0; forceField = 0;
	shield = 0; repairs = 0;
	sensors = 0;
	brain = 0;
	stolen = 0; wasted = 0;
}

void GBExpenditureStatistics::ReportConstruction(const GBEnergy en) { construction += ToDouble(en);}
void GBExpenditureStatistics::ReportEngine(const GBEnergy en) { engine += ToDouble(en);}
void GBExpenditureStatistics::ReportForceField(const GBEnergy en) { forceField += ToDouble(en);}
void GBExpenditureStatistics::ReportWeapons(const GBEnergy en) { weapons += ToDouble(en);}
void GBExpenditureStatistics::ReportShield(const GBEnergy en) { shield += ToDouble(en);}
void GBExpenditureStatistics::ReportRepairs(const GBEnergy en) { repairs += ToDouble(en);}
void GBExpenditureStatistics::ReportSensors(const GBEnergy en) { sensors += ToDouble(en);}
void GBExpenditureStatistics::ReportBrain(const GBEnergy en) { brain += ToDouble(en);}
void GBExpenditureStatistics::ReportStolen(const GBEnergy en) { stolen += ToDouble(en);}
void GBExpenditureStatistics::ReportWasted(const GBEnergy en) { wasted += ToDouble(en);}

long GBExpenditureStatistics::Construction() const { return construction; }
long GBExpenditureStatistics::Engine() const { return engine; }
long GBExpenditureStatistics::Weapons() const { return weapons; }
long GBExpenditureStatistics::ForceField() const { return forceField; }
long GBExpenditureStatistics::Shield() const { return shield; }
long GBExpenditureStatistics::Repairs() const { return repairs; }
long GBExpenditureStatistics::Sensors() const { return sensors; }
long GBExpenditureStatistics::Brain() const { return brain; }
long GBExpenditureStatistics::Stolen() const { return stolen; }
long GBExpenditureStatistics::Wasted() const { return wasted; }

long GBExpenditureStatistics::Total() const {
	return Construction() + Engine()
		+ Weapons() + ForceField() + Shield() + Repairs()
		+ Sensors() + Brain() + Stolen() + Wasted();
}

GBExpenditureStatistics & GBExpenditureStatistics::operator +=(const GBExpenditureStatistics & other) {
	construction += other.construction;
	engine += other.engine;
	weapons += other.weapons;
	forceField += other.forceField;
	shield += other.shield;
	repairs += other.repairs;
	sensors += other.sensors;
	brain += other.brain;
	stolen += other.stolen;
	wasted += other.wasted;
	return *this;
}

// GBScores //

GBScores::GBScores()
	: rounds(0), sides(0), survived(0), sterile(0), earlyDeaths(0), elimination(0),
	population(0), populationEver(0),
	biomass(0), earlyBiomass(0),
	constructor(0), economyHardware(0), combatHardware(0), totalHardware(0),
	territory(0),
	dead(0), killed(0), suicide(0),
	damageDone(0), damageTaken(0), friendlyFire(0),
	income(), expenditure(), seeded(0),
	biomassFraction(0.0), earlyBiomassFraction(0.0),
	killedFraction(0.0),
	biomassFractionSquared(0.0)
{
	biomassHistory.resize(1, 0);
}

GBScores::~GBScores() {}

void GBScores::Reset() {
	rounds = sides = 0;
	survived = sterile = earlyDeaths = elimination = 0;
	population = populationEver = 0;
	biomass = earlyBiomass = 0;
	constructor = 0;
	economyHardware = combatHardware = totalHardware = 0;
	territory = 0;
	seeded = 0;
	income.Reset();
	expenditure.Reset();
	dead = killed = suicide = 0;
	damageDone = damageTaken = friendlyFire = 0;
	biomassFraction = earlyBiomassFraction = 0.0;
	killedFraction = 0.0;
	biomassFractionSquared = 0.0;
	biomassHistory.resize(1, 0);
	biomassHistory[0] = 0;
}

void GBScores::OneRound() { rounds = 1; }

GBScores & GBScores::operator +=(const GBScores & other) {
	rounds += other.rounds;
	sides += other.sides;
	survived += other.survived;
	sterile += other.sterile;
	earlyDeaths += other.earlyDeaths;
	elimination += other.elimination;
	population += other.population;
	populationEver += other.populationEver;
	biomass += other.biomass;
	earlyBiomass += other.earlyBiomass;
	constructor += other.constructor;
	economyHardware += other.economyHardware;
	combatHardware += other.combatHardware;
	totalHardware += other.totalHardware;
	territory += other.territory;
	seeded += other.seeded;
	income += other.income;
	expenditure += other.expenditure;
	dead += other.dead;
	killed += other.killed;
	suicide += other.suicide;
	damageDone += other.damageDone;
	damageTaken += other.damageTaken;
	friendlyFire += other.friendlyFire;
	biomassFraction += other.biomassFraction;
	earlyBiomassFraction += other.earlyBiomassFraction;
	killedFraction += other.killedFraction;
	biomassFractionSquared += other.biomassFractionSquared;
//add biomass
	if (biomassHistory.size() < other.biomassHistory.size())
		biomassHistory.resize(other.biomassHistory.size(), biomassHistory.back());
	int osize = other.biomassHistory.size();
	for ( int i = osize; i < biomassHistory.size(); ++ i )
		biomassHistory[i] += other.biomassHistory.back();
	for ( int i = 0; i < osize; ++ i )
		biomassHistory[i] += other.biomassHistory[i];
	return *this;
}

long GBScores::Sides() const {
	return sides;}

long GBScores::Rounds() const {
	return rounds;}

long GBScores::Survived() const {
	return survived;}

long GBScores::Sterile() const {
	return sterile;}

long GBScores::EarlyDeaths() const {
	return earlyDeaths;}

long GBScores::SurvivedEarly() const {
	return sides - earlyDeaths;}

float GBScores::Survival() const {
	return sides ? (float)survived / sides : 0;}

float GBScores::SurvivalNotSterile() const {
	return sides ? 1.0 - (float)sterile / sides : 0;}

float GBScores::EarlyDeathRate() const {
	return sides ? (float)earlyDeaths / sides : 0;}

float GBScores::LateDeathRate() const {
	if ( sides - earlyDeaths <= 0 ) return 0;
	return 1.0 - (float)(sides - sterile) / (sides - earlyDeaths);
}

long GBScores::Elimination() const { return elimination; }

float GBScores::EliminationRate() const {
	return rounds ? (float)elimination / rounds : 0;}

long GBScores::Population() const {
	return population / (rounds ? rounds : 1);}

long GBScores::PopulationEver() const {
	return populationEver / (rounds ? rounds : 1);}

long GBScores::Biomass() const { return biomass / rounds; }
long GBScores::EarlyBiomass() const { return earlyBiomass / rounds; }
long GBScores::SurvivalBiomass() const { return biomass / survived; }
long GBScores::EarlySurvivalBiomass() const {
	return earlyBiomass / (rounds - earlyDeaths);}

float GBScores::BiomassFraction() const {
	return biomassFraction / (rounds ? rounds : 1);}

float GBScores::EarlyBiomassFraction() const {
	return earlyBiomassFraction / (rounds ? rounds : 1);}

float GBScores::SurvivalBiomassFraction() const {
	return biomassFraction / (survived ? survived : 1);}

const std::vector<long> GBScores::BiomassHistory() const {
	if ( rounds <= 1 )
		return biomassHistory;
	std::vector<long> avg = biomassHistory;
	for ( int i = 0; i < avg.size(); ++i )
		avg[i] /= rounds;
	return avg;
}

long GBScores::Constructor() const { return rounds ? constructor / rounds : 0; }

long GBScores::Territory() const { return territory; }

GBRatio GBScores::EconFraction() const { return economyHardware / totalHardware; }
GBRatio GBScores::CombatFraction() const { return combatHardware / totalHardware; }


long GBScores::Seeded() const { return rounds ? seeded / rounds : 0; }

GBIncomeStatistics & GBScores::Income() { return income;}
const GBIncomeStatistics & GBScores::Income() const { return income;}

GBExpenditureStatistics & GBScores::Expenditure() { return expenditure;}
const GBExpenditureStatistics & GBScores::Expenditure() const { return expenditure;}

long GBScores::Dead() const {
	return dead / rounds;}

long GBScores::Killed() const {
	return killed / rounds;}

long GBScores::Suicide() const {
	return suicide / rounds;}

float GBScores::KilledFraction() const {
	return killedFraction / (rounds ? rounds : 1);}

float GBScores::KillRate() const {
	if ( ! biomass ) return 0.0;
	return killed / biomass;
}

//What fraction of income has ended up as growth?
float GBScores::Efficiency() const {
	if ( ! income.Total() ) return 0.0;
	return (biomass - seeded) / Income().Total();
}

GBFrames GBScores::Doubletime(GBFrames currentTime) const {
	if ( ! seeded || biomass <= 1 ) return 0;
	return (GBFrames)(currentTime * log(2.0) / log(biomass) / seeded);
}

float GBScores::BiomassFractionSD() const {
	float frac = BiomassFraction();
	if (!rounds) return 0.0;
	float variance = biomassFractionSquared / rounds - frac * frac;
	return variance < 0 ? 0 : sqrt(variance); //rounding error can make variance slightly negative when it should be zero
}

//Sampling error: twice the standard deviation of the mean.
float GBScores::BiomassFractionError() const {
	return rounds > 1 ? BiomassFractionSD() / sqrt((float)(rounds - 1)) * 2.0 : 1.0;
}

// GBSideScores //

GBSideScores::GBSideScores()
	: GBScores(), extinctTime(0), sterileTime(0)
{}

GBSideScores::~GBSideScores() {}

void GBSideScores::ResetSampledStatistics() {
	population = 0;
	biomass = 0;
	constructor = economyHardware = combatHardware = totalHardware = 0;
	
	territory = 0;
}

void GBSideScores::Reset() {
	GBScores::Reset();
	extinctTime = 0;
	sterileTime = 0;
}

void GBSideScores::ReportRobot(const GBEnergy &botBiomass, const GBEnergy &construc,
							   const GBEnergy &econ, const GBEnergy &combat, const GBEnergy hw) {
	population += 1;
	biomass += ToDouble(botBiomass);
	constructor += ToDouble(construc);
	economyHardware += ToDouble(econ);
	combatHardware += ToDouble(combat);
	totalHardware += ToDouble(hw);
}

void GBSideScores::ReportDead(const GBEnergy en) { dead += ToDouble(en); }
void GBSideScores::ReportKilled(const GBEnergy en) { killed += ToDouble(en); }
void GBSideScores::ReportSuicide(const GBEnergy en) { suicide += ToDouble(en); }

void GBSideScores::ReportDamageDone(const GBDamage d) { damageDone += ToDouble(d); }
void GBSideScores::ReportDamageTaken(const GBDamage d) { damageTaken += ToDouble(d); }
void GBSideScores::ReportFriendlyFire(const GBDamage d) { friendlyFire += ToDouble(d); }

void GBSideScores::ReportSeeded(const GBEnergy en) {
	seeded += ToDouble(en);
	if ( biomassHistory.size() == 1 )
		biomassHistory[0] = seeded;
	sides = 1;
	rounds = 1;
}

void GBSideScores::ReportTerritory() { ++ territory; }

void GBSideScores::ReportTotals(const GBScores & totals) {
	biomassFraction = totals.Biomass() ? biomass / totals.Biomass() : 0.0;
	biomassFractionSquared = biomassFraction * biomassFraction;
	earlyBiomassFraction = totals.EarlyBiomass() ? earlyBiomass / totals.EarlyBiomass() : 0.0;
	killedFraction = totals.Killed() ? killed / totals.Killed() : 0.0;
	if ( totals.Survived() == 1 && survived ) elimination = 1;
}

void GBSideScores::ReportFrame(const GBFrames frame) {
    if ( ! seeded ) return;
	if ( population ) {
		extinctTime = 0;
		survived = 1;
		if ( constructor <= kMaxSterileConstructor ) {
			if ( ! sterile ) {
				sterile = 1;
				sterileTime = frame;
			}
		} else {
			sterile = 0;
			sterileTime = 0;
		}
	} else if ( ! extinctTime ) {
		extinctTime = frame;
		if ( ! sterileTime ) sterileTime = frame;
		survived = 0;
		sterile = 1;
	}
	if ( frame == kEarlyDeathTime ) {
		earlyBiomass = biomass;
		if ( sterile ) earlyDeaths = 1;
	}
	if ( frame % 100 == 0 && frame )
		biomassHistory.push_back(biomass);
}

GBFrames GBSideScores::ExtinctTime() const { return extinctTime; }
GBFrames GBSideScores::SterileTime() const { return sterileTime; }

long GBSideScores::GetNewRobotNumber() {
	return ++ populationEver; // preincrement for 1-based numbering
}

