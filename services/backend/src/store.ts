import type { MatchResult, PlayerProfile } from "./types";

const DEFAULT_PROFILE = (playerId: string): PlayerProfile => ({
  playerId,
  displayName: playerId,
  level: 1,
  xp: 0,
  credits: 0,
  loadout: {
    primaryWeaponId: "AR15",
    secondaryWeaponId: "Pistol",
    lethalEquipmentId: "Frag",
    tacticalEquipmentId: "Flashbang",
    primaryAttachments: [],
  },
  perks: {
    tier1Perk: "Sprinter",
    tier2Perk: "SteadyHands",
    tier3Perk: "Endurance",
  },
  updatedAt: new Date().toISOString(),
});

export class InMemoryStore {
  private readonly profiles = new Map<string, PlayerProfile>();
  private readonly matchResults = new Map<string, MatchResult>();
  private readonly reports: Array<{ reporterId: string; reportedId: string; reason: string; createdAt: string }> = [];

  getProfile(playerId: string): PlayerProfile {
    const existing = this.profiles.get(playerId);
    if (existing) {
      return existing;
    }

    const profile = DEFAULT_PROFILE(playerId);
    this.profiles.set(playerId, profile);
    return profile;
  }

  putProfile(profile: PlayerProfile): PlayerProfile {
    const normalized = { ...profile, updatedAt: new Date().toISOString() };
    this.profiles.set(profile.playerId, normalized);
    return normalized;
  }

  applyXP(playerId: string, xpDelta: number): PlayerProfile {
    const profile = this.getProfile(playerId);
    const updatedXP = Math.max(0, profile.xp + xpDelta);
    const updatedLevel = Math.max(1, Math.floor(updatedXP / 1000) + 1);
    return this.putProfile({ ...profile, xp: updatedXP, level: updatedLevel });
  }

  grantCredits(playerId: string, creditsDelta: number): PlayerProfile {
    const profile = this.getProfile(playerId);
    return this.putProfile({ ...profile, credits: Math.max(0, profile.credits + creditsDelta) });
  }

  saveMatchResult(result: MatchResult): MatchResult {
    this.matchResults.set(result.matchId, result);
    return result;
  }

  reportPlayer(reporterId: string, reportedId: string, reason: string): void {
    this.reports.push({
      reporterId,
      reportedId,
      reason,
      createdAt: new Date().toISOString(),
    });
  }
}

export const store = new InMemoryStore();
