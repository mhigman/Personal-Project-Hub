/** @type {import('next').NextConfig} */
const nextConfig = {
    images: {
        remotePatterns: [
          {
            protocol: 'https',
            hostname: 'h7cy0nchcgetswyy.public.blob.vercel-storage.com',
            port: '',
          },
        ],
      },
}

module.exports = nextConfig
