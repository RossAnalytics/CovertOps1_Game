import "dotenv/config";
import cors from "cors";
import express from "express";
import { randomUUID } from "crypto";
import { z } from "zod";
import { eosAuth, type AuthenticatedRequest } from "./eosAuth";
import { store } from "./store";
import type { MatchMode, MatchResult, PerkSelection, PlayerProfile } from "./types";

const app = express();
app.use(cors());
app.use(express.json({ limit: "2mb" }));

const loadoutSchema = z.object({
  primaryWeaponId: z.string().min(1),
  secondaryWeaponId: z.string().min(1),
  lethalEquipmentId: z.string().min(1),
  tacticalEquipmentId: z.string().min(1),
  primaryAttachments: z.array(z.string()).max(6),
});

const perksSchema = z.object({
  tier1Perk: z.string().min(1),
  tier2Perk: z.string().min(1),
  tier3Perk: z.string().min(1),
});

const profileSchema = z.object({
  playerId: z.string().min(1),
  displayName: z.string().min(1),
  level: z.number().int().min(1),
  xp: z.number().int().min(0),
  credits: z.number().int().min(0),
  loadout: loadoutSchema,
  perks: perksSchema,
  updatedAt: z.string(),
});

const matchTicketSchema = z.object({
  mode: z.enum(["TDM", "CTF", "SnD"]),
  region: z.string().min(1),
  partySize: z.number().int().min(1).max(6),
  skillRating: z.number().min(0),
});

const matchResultSchema: z.ZodType<MatchResult> = z.object({
  matchId: z.string().min(1),
  mode: z.enum(["TDM", "CTF", "SnD"]) as z.ZodType<MatchMode>,
  durationSeconds: z.number().min(1),
  winningTeam: z.number().int().min(0),
  teamScores: z.array(z.number().int().min(0)).min(2),
  participants: z.array(
    z.object({
      playerId: z.string().min(1),
      teamId: z.number().int().min(0),
      kills: z.number().int().min(0),
      deaths: z.number().int().min(0),
      assists: z.number().int().min(0),
      score: z.number().int().min(0),
      objectiveActions: z.number().int().min(0),
    }),
  ),
});

const xpGrantSchema = z.object({
  playerId: z.string().min(1),
  amount: z.number().int().min(1),
  source: z.string().min(1),
});

const creditsGrantSchema = z.object({
  playerId: z.string().min(1),
  amount: z.number().int().min(1),
  source: z.string().min(1),
});

const reportSchema = z.object({
  reportedPlayerId: z.string().min(1),
  reason: z.string().min(3).max(500),
  matchId: z.string().optional(),
});

app.get("/health", (_req, res) => {
  res.json({ ok: true, service: "covertops1-backend", now: new Date().toISOString() });
});

// POST /matchmaking/tickets
app.post("/matchmaking/tickets", eosAuth, (req: AuthenticatedRequest, res) => {
  const parsed = matchTicketSchema.safeParse(req.body);
  if (!parsed.success) {
    res.status(400).json({ error: parsed.error.flatten() });
    return;
  }

  res.status(201).json({
    ticketId: randomUUID(),
    playerId: req.auth?.playerId ?? "unknown",
    ...parsed.data,
    createdAt: new Date().toISOString(),
    status: "QUEUED",
  });
});

// POST /match/results
app.post("/match/results", eosAuth, (req, res) => {
  const parsed = matchResultSchema.safeParse(req.body);
  if (!parsed.success) {
    res.status(400).json({ error: parsed.error.flatten() });
    return;
  }

  const result = store.saveMatchResult(parsed.data);
  res.status(201).json(result);
});

// GET /profiles/{playerId}
app.get("/profiles/:playerId", eosAuth, (req, res) => {
  res.json(store.getProfile(req.params.playerId));
});

// PUT /profiles/{playerId}
app.put("/profiles/:playerId", eosAuth, (req, res) => {
  const candidate = {
    ...(req.body as Partial<PlayerProfile>),
    playerId: req.params.playerId,
  };

  const parsed = profileSchema.safeParse(candidate);
  if (!parsed.success) {
    res.status(400).json({ error: parsed.error.flatten() });
    return;
  }

  res.json(store.putProfile(parsed.data));
});

// POST /progression/xp
app.post("/progression/xp", eosAuth, (req, res) => {
  const parsed = xpGrantSchema.safeParse(req.body);
  if (!parsed.success) {
    res.status(400).json({ error: parsed.error.flatten() });
    return;
  }

  const profile = store.applyXP(parsed.data.playerId, parsed.data.amount);
  res.status(201).json({
    playerId: profile.playerId,
    level: profile.level,
    xp: profile.xp,
    source: parsed.data.source,
  });
});

// POST /economy/credits/grant
app.post("/economy/credits/grant", eosAuth, (req, res) => {
  const parsed = creditsGrantSchema.safeParse(req.body);
  if (!parsed.success) {
    res.status(400).json({ error: parsed.error.flatten() });
    return;
  }

  const profile = store.grantCredits(parsed.data.playerId, parsed.data.amount);
  res.status(201).json({
    playerId: profile.playerId,
    credits: profile.credits,
    source: parsed.data.source,
  });
});

// POST /reports/player
app.post("/reports/player", eosAuth, (req: AuthenticatedRequest, res) => {
  const parsed = reportSchema.safeParse(req.body);
  if (!parsed.success) {
    res.status(400).json({ error: parsed.error.flatten() });
    return;
  }

  store.reportPlayer(req.auth?.playerId ?? "unknown", parsed.data.reportedPlayerId, parsed.data.reason);
  res.status(201).json({ status: "RECORDED" });
});

const port = Number(process.env.PORT ?? 8080);
app.listen(port, () => {
  // eslint-disable-next-line no-console
  console.log(`CovertOps1 backend listening on ${port}`);
});
