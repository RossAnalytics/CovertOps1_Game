import type { NextFunction, Request, Response } from "express";

const bypassEnabled = String(process.env.EOS_DEV_AUTH_BYPASS ?? "true").toLowerCase() === "true";

export interface AuthenticatedRequest extends Request {
  auth?: {
    playerId: string;
    tokenSource: "dev-bypass" | "bearer";
  };
}

export function eosAuth(req: AuthenticatedRequest, res: Response, next: NextFunction): void {
  const header = req.header("authorization");
  if (!header) {
    if (!bypassEnabled) {
      res.status(401).json({ error: "Missing authorization header" });
      return;
    }

    req.auth = {
      playerId: "dev-player",
      tokenSource: "dev-bypass",
    };
    next();
    return;
  }

  const [scheme, token] = header.split(" ");
  if (scheme?.toLowerCase() !== "bearer" || !token) {
    res.status(401).json({ error: "Invalid authorization header format" });
    return;
  }

  // EOS JWT verification should be implemented against EOS issuer metadata/JWKS.
  // For initial dev integration this accepts any bearer token and treats it as player id.
  req.auth = {
    playerId: token,
    tokenSource: "bearer",
  };
  next();
}
