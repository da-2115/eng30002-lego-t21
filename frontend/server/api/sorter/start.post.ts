import { useSorterState } from "../utils/sorter-state";

export default defineEventHandler(async () => {
  const state = useSorterState();

  if (state.mode === "running") {
    throw createError({
      statusCode: 409,
      statusMessage: "Sorter is already running",
    });
  }

  // Later:
  // await hardwareClient.startSorter()

  state.mode = "running";
  state.sessionId = crypto.randomUUID();
  state.startedAt = new Date().toISOString();
  state.bricksProcessed = 0;
  state.bricksPerMinute = 0;
  state.lastError = null;

  return {
    success: true,
    state,
  };
});
