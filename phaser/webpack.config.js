const path = require('path')
const MiniCssExtractPlugin = require("mini-css-extract-plugin")
const CopyWebpackPlugin = require('copy-webpack-plugin')
const webpack = require('webpack')

module.exports = {
    entry: './src/index.ts',

    plugins: [
        function() {
            this.hooks.done.tap('BuildStatsPlugin', function() {
                const timestamp = {
                    buildTime: new Date().toISOString().replace(/T/, ' ').replace(/\..+/, '')
                }

                const fs = require('fs')

                fs.writeFileSync('dist/build-stats.json', JSON.stringify(timestamp, null, 2))
            })
        },
        new MiniCssExtractPlugin(),
        new CopyWebpackPlugin({
            patterns: [
                {
                    from: path.resolve(__dirname, 'static'),
                }, // Adjust the source directory as needed
            ],
        }),
        new webpack.DefinePlugin({
            __BUILD_TIME__: JSON.stringify(new Date().toISOString().replace(/T/, ' ').replace(/\..+/, ''))
        }),
    ],

    module: {
        rules: [
            {
                test: /\.tsx?$/,
                use: 'ts-loader',
                exclude: /node_modules/,
            },
            {
                test: /\.css$/,
                use: [
                    MiniCssExtractPlugin.loader, // instead of style-loader
                    'css-loader'
                ]
            }
        ],
    },

    resolve: {
        extensions: ['.tsx', '.ts', '.js', '.css'],
    },

   output: {
        path: path.resolve(__dirname, 'dist'),
        // publicPath: './static',
        filename: 'wishlair1.js',
        clean: true,
    }
}

