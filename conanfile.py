from conans import ConanFile

class PBE(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    version = "2.1.0"
    license = "BSD"
    author = "NHERI SimCenter"
    url = "https://github.com/NHERI-SimCenter/PBE"
    description = "NHERI SimCenter Performance-Based Engineering Tool"
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake"
    requires = "jansson/2.11@bincrafters/stable", \
               "libcurl/7.64.1@bincrafters/stable", \
               "lapack/3.7.1@conan/stable", \
               "hdf5/1.12.0",\
               "zlib/1.2.11@conan/stable"

    build_policy = "missing"


    def configure(self):
        if self.settings.os == "Windows":
            self.options["libcurl"].with_winssl = True
            self.options["libcurl"].with_openssl = False
            self.options["lapack"].visual_studio = True
            self.options["lapack"].shared = True

    def imports(self):
        if self.settings.os == "Windows":
            output = './%s' % self.settings.build_type
            self.copy("lib*.dll", dst=output, src="bin")
