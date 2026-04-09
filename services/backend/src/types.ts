export type MatchMode = "TDM" | "CTF" | "SnD";

export interface LoadoutSpec {
  primaryWeaponId: string;
  secondaryWeaponId: string;
  lethalEquipmentId: string;
  tacticalEquipmentId: string;
  primaryAttachments: string[];
}

export interface PerkSelection {
  tier1Perk: string;
  tier2Perk: string;
  tier3Perk: string;
}

export interface PlayerProfile {
  playerId: string;
  displayName: string;
  level: number;
  xp: number;
  credits: number;
  loadout: LoadoutSpec;
  perks: PerkSelection;
  updatedAt: string;
}

export interface MatchResult {
  matchId: string;
  mode: MatchMode;
  durationSeconds: number;
  winningTeam: number;
  teamScores: number[];
  participants: Array<{
    playerId: string;
    teamId: number;
    kills: number;
    deaths: number;
    assists: number;
    score: number;
    objectiveActions: number;
  }>;
}
