# CovertOps1 Backend Service

Initial multiplayer backend scaffold for profiles, progression, economy, match submissions, and player reporting.

## Endpoints (v1)
- `POST /matchmaking/tickets`
- `POST /match/results`
- `GET /profiles/:playerId`
- `PUT /profiles/:playerId`
- `POST /progression/xp`
- `POST /economy/credits/grant`
- `POST /reports/player`

## Local Run
```powershell
cd services/backend
copy .env.example .env
npm install
npm run dev
```

The service uses an in-memory store by default. Replace `src/store.ts` with Postgres/Redis adapters for production.
