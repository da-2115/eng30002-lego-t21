export default defineEventHandler(async () => {
    // Replace these mocked values with health checks later
    return {
        status: 'online',
        timestamp: new Date().toISOString(),
        services: {
            nuxtApi: {
                connected: true
            },
            hardwareController: {
                connected: false
            },
            camera: {
                connected: false
            },
            identificationService: {
                connected: false
            }
        }
    }
})