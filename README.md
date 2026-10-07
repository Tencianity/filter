#Tencianity's Filter

<!-- Markdown formatting for "reference style" links: courtesy of https://github.com/othneildrew/Best-README-Template -->
<!-- Additionally most inspiration for the formatting of this README.md can be dedicated to the Best-README-Template project -->

[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]


<details id="table-of-contents">
	<summary>Table of Contents</summary>
	<ol>
		<li><a href="#description">Description</a></li>
		<li><a href="#usage">Usage</a></li>
	</ol>
</details>

##Description
<article style="text-align: left;" id="description">
	<p style="color: gold">
		The goal of this project is to provide an intuitive CLI for adding various	<br>
		color filters to images of all types!
	</p>
	<p>
		Currently it is able to handle BMP and PNG file types with support for JPG	<br>
		still being a WIP. Current applicable color and processing filters you can	<br>
		apply to your images include:
	</p>
	<dl>
		<dt>Red Shift</dt>
		<dd>Makes all the pixels of the image more red.</dd>
		<dt>Green Shift</dt>
		<dd>Makes all the pixels of the image more green.</dd>
		<dt>Blue Shift</dt>
		<dd>Makes all the pixels of the image more blue.</dd>
		<dt>Blur</dt>
		<dd>
			Make the image more blurry by shifting each pixel's RGB values
			closer to the average of its neighbors RGB values.
		</dd>
		<dt>Greyscale</dt>
		<dd>Puts the image in greyscale.</dd>
		<dt>Reflect</dt>
		<dd>Reflects the image, flipping it from left to right.</dd>
	</dl>
</article>

##Usage
<article id="usage">
	<p>./filter [OPTIONS] infile outfile</p>
  <p>-b, --blur         applies blur filter to image</p>
  <p>-g, --greyscale    applies greyscale filter to image</p>
  <p>-r, --reflect      applies reflect filter to image</p>
  <p>-s, --sepia        applies sepia filter to image</p>
  <p>-R, --red          applies red-shift filter to image</p>
  <p>-G, --green        applies green-shift filter to image</p>
  <p>-B, --blue         applies blue-shift filter to image</p>
</article>

<!-- Markdown "reference style" links format courtesy of https://github.com/othneildrew/Best-README-Template -->
[forks-shield]: https://img.shields.io/github/forks/Tencianity/filter.svg?style=for-the-badge
[forks-url]: https://github.com/Tencianity/filter/network/members
[stars-shield]: https://img.shields.io/github/stars/Tencianity/filter.svg?style=for-the-badge
[stars-url]: https://github.com/Tencianity/filter/stargazers
[issues-shield]: https://img.shields.io/github/issues/Tencianity/filter.svg?style=for-the-badge
[issues-url]: https://github.com/Tencianity/filter/issues

