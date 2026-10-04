// server/api/recognize.ts

export default defineEventHandler(async (event) => {
  try {
    const data = await $fetch('http://localhost:8080/api/capture-and-recognize', {
      method: 'GET',
      timeout: 15000 // Give the C++ OpenSSL engine up to 15 seconds to parse the file
    })
    
    return data
  } catch (error: any) {
    // This safely forwards errors without causing frontend 405 Method mismatches
    throw createError({
      statusCode: 502,
      statusMessage: 'Bad Gateway',
      message: 'Nuxt Nitro failed to bridge over to the C++ background daemon.',
      data: {
        errorName: error.name,
        errorMessage: error.message,
        errorCode: error.code || 'UNKNOWN_SOCKET_ERROR'
      }
    })
  }
})
