import mariadb from 'mariadb'

const config = useRuntimeConfig()

const pool = mariadb.createPool({
  host: config.dbHost || process.env.DB_HOST || 'localhost',
  user: config.dbUser || process.env.DB_USER || 'root',
  password: config.dbPassword || process.env.DB_PASSWORD,
  database: config.dbDatabase || process.env.DB_DATABASE || 'test',
  connectionLimit: 10,
  acquireTimeout: 10000,
})

export const db = {
  /**
   * Executes a MariaDB SQL query against the pool.
   * @param text The SQL query string (use ? for placeholders)
   * @param params Array of values to safely map to ? placeholders
   */
  async query(text: string, params?: any[]) {
    const start = Date.now()
    let conn
    
    try {
      conn = await pool.getConnection()
      const res = await conn.query(text, params)
      
      if (process.env.NODE_ENV === 'development') {
        console.log('Executed MariaDB query', { text, duration: Date.now() - start })
      }
      
      return res
    } catch (error) {
      console.error('Database query error:', error)
      throw error
    } finally {
      if (conn) conn.release()
    }
  },
  
  pool,
}
