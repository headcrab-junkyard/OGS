This is a draft version of the OpenGoldSource engine installation notes

## Building:

### Lazy/easy way:
* Clone the repo via the git program;
* Initialize and update all the repo submodules;
* Open the cmake-gui and customize the build for your needs;
* Build it;

### Alternative (manual) way:
* Create a root folder somewhere on your HDD and call it somehow (ex: "headcrab-garage"); All other downloaded archives should be unpacked inside this folder;
* Download the [repo](https://github.com/headcrab-garage/OpenGoldSrc) archive and unpack it in root folder;
* Download the [hlsdk](https://github.com/headcrab-garage/hlsdk-core) repo archive and unpack it;
* Download the [goldsrc-sdk](https://github.com/headcrab-garage/goldsrc-sdk) repo and unpack it;
* Open the cmake-gui and customize the build for your needs. You also need to provide paths to all required dependencies (hlsdk, goldsrc-sdk, etc); You need to manually change the path for OGS_HLSDK_PATH option to point to "../hlsdk-core" instead of "external" folder and OGS_GOLDSRCSDK_PATH to point to "../goldsrc-sdk" instead of "external";
* Build it;