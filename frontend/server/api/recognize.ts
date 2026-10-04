// server/api/recognize.ts

export default defineEventHandler(async () => {
  let response: Response;

  try {
    response = await fetch("http://127.0.0.1:8080/api/capture-and-recognize", {
      method: "GET",
      signal: AbortSignal.timeout(60_000),
    });
  } catch (error: unknown) {
    throw createError({
      statusCode: 502,
      statusMessage: "Bad Gateway",
      message: "Could not connect to the C++ backend.",
      data: {
        detail: error instanceof Error ? error.message : String(error),
      },
    });
  }

  const body = await response.text();

  if (!response.ok) {
    throw createError({
      statusCode: response.status,
      message: "C++ recognition request failed.",
      data: { upstreamBody: body },
    });
  }

  try {
    return JSON.parse(body);
  } catch {
    throw createError({
      statusCode: 502,
      message: "C++ backend returned invalid JSON.",
      data: { upstreamBody: body },
    });
  }
});
