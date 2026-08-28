import { useDetectionStore } from '../utils/detection-store'

export default defineEventHandler(() => {
    return useDetectionStore()
})