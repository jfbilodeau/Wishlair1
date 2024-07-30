const path = require('path')
// const HtmlWebpackPlugin = require('html-webpack-plugin')
const MiniCssExtractPlugin = require("mini-css-extract-plugin")
const CopyWebpackPlugin = require('copy-webpack-plugin')

module.exports = {
    entry: './src/index.ts',

    plugins: [
        // new HtmlWebpackPlugin({
        //     title: 'Wishlair 1',
        // }),
        new MiniCssExtractPlugin(),
        new CopyWebpackPlugin({
            patterns: [
                {
                    from: path.resolve(__dirname, 'static'),
                }, // Adjust the source directory as needed
            ],
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